#include <cassert>
#include <cstring>

#include "CASMySQLBind.h"
#include "CASMySQLPreparedStatement.h"

#include "CASMySQLResultSet.h"

CASMySQLResultSet::CASMySQLResultSet( CASMySQLPreparedStatement* pStatement )
{
	assert( pStatement );

	m_pStatement = pStatement;

	m_pStatement->AddRef();

	const int iMaxLength = 1;
	mysql_stmt_attr_set( pStatement->GetStatement(), STMT_ATTR_UPDATE_MAX_LENGTH, &iMaxLength );

	if( mysql_stmt_store_result( m_pStatement->GetStatement() ) == 0 )
	{
		m_pResultSet = mysql_stmt_result_metadata( m_pStatement->GetStatement() );

		m_pFields = mysql_fetch_fields( m_pResultSet );

		const int iFieldCount = GetFieldCount();

		if( iFieldCount > 0 )
		{
			m_pBinds = new MYSQL_BIND[ iFieldCount ];

			memset( m_pBinds, 0, sizeof( MYSQL_BIND ) * iFieldCount );

			m_pVariables = new CASMySQLBind[ iFieldCount ];

			for( int iIndex = 0; iIndex < iFieldCount; ++iIndex )
			{
				m_pVariables[ iIndex ].SetOutput( m_pFields[ iIndex ], &m_pBinds[ iIndex ] );
			}

			mysql_stmt_bind_result( m_pStatement->GetStatement(), m_pBinds );
		}
	}
	else
	{
		//TODO: log error - Solokiller
	}
}

CASMySQLResultSet::~CASMySQLResultSet()
{
	if( m_pResultSet )
	{
		mysql_free_result( m_pResultSet );
	}

	delete[] m_pBinds;

	mysql_stmt_free_result( m_pStatement->GetStatement() );

	delete[] m_pVariables;

	m_pStatement->Release();
}

int CASMySQLResultSet::GetFieldCount() const
{
	return mysql_stmt_field_count( m_pStatement->GetStatement() );
}

bool CASMySQLResultSet::Next()
{
	return mysql_stmt_fetch( m_pStatement->GetStatement() ) == 0;
}

int32_t CASMySQLResultSet::GetSigned32( int iColumn ) const
{
	if( iColumn < 0 || iColumn >= GetFieldCount() )
		return 0;

	return m_pVariables[ iColumn ].m_iVal32;
}