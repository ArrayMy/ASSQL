#include <cassert>

#include "CASSQLiteQuery.h"

#include "CASSQLiteConnection.h"

CASSQLiteConnection::CASSQLiteConnection( CASSQLThreadPool& pool, const char* const pszFilename )
	: m_ThreadPool( pool )
{
	assert( pszFilename );

	const int iResult = sqlite3_open( pszFilename, &m_pConnection );

	if( SQLITE_OK != iResult )
	{
		m_pConnection = nullptr;

		GetLogFunction()( "SQLiteConnection::SQLiteConnection: %s\n", sqlite3_errstr( iResult ) );
	}
}

CASSQLiteConnection::~CASSQLiteConnection()
{
	Close();
}

bool CASSQLiteConnection::IsOpen() const
{
	return m_pConnection != nullptr;
}

void CASSQLiteConnection::Close()
{
	if( IsOpen() )
	{
		sqlite3_close( m_pConnection );
		m_pConnection = nullptr;
	}
}

bool CASSQLiteConnection::Query( const std::string& szQuery, asIScriptFunction* const pCallback )
{
	auto pQuery = new CASSQLiteQuery( this, szQuery.c_str() );

	bool bSuccess = false;

	if( pQuery->IsValid() )
	{
		bSuccess = m_ThreadPool.AddItem( pQuery, pCallback );
	}

	if( pCallback )
		pCallback->Release();

	pQuery->Release();

	return bSuccess;
}

CASSQLitePreparedStatement* CASSQLiteConnection::CreatePreparedStatement( const std::string& szStatement )
{
	auto pStatement = new CASSQLitePreparedStatement( this, szStatement.c_str() );

	if( pStatement->IsValid() )
	{
		return pStatement;
	}

	pStatement->Release();

	return nullptr;
}