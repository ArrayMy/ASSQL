
class Database
{
	private MySQLConnection@ m_pConnection = null;
	
	void Connect()
	{
		@m_pConnection = SQL.CreateMySQLConnection( "localhost", "root", "" );
		
		if( m_pConnection !is null )
		{
			bool bSuccess = m_pConnection.Query(
				"CREATE DATABASE IF NOT EXISTS TestDB;",
				SQLQueryCallback( this.CreatedDB )
			);
			
			Print( "Created database query: %1\n", bSuccess ? "yes" : "no" );
		}
	}
	
	private void CreatedDB( SQLQuery@ pQuery )
	{
		bool bSuccess = m_pConnection.Query(
			"USE TestDB;",
			SQLQueryCallback( this.SelectedDB )
		);
		
		Print( "Selected database query: %1\n", bSuccess ? "yes" : "no" );
	}
	
	private void SelectedDB( SQLQuery@ pQuery )
	{
		bool bSuccess = m_pConnection.Query( 
			"CREATE TABLE IF NOT EXISTS Test("
			"ID INT PRIMARY KEY NOT NULL,"
			"VALUE INT NOT NULL,"
			"STRING TEXT NOT NULL,"
			"date DATE NOT NULL,"
			"time TIME NOT NULL,"
			"datetime DATETIME NOT NULL"
			")",
			SQLQueryCallback( this.CreatedTable )
		);
		
		Print( "Created query: %1\n", bSuccess ? "yes" : "no" );
	}
	
	private void CreatedTable( SQLQuery@ pQuery )
	{
		MySQLPreparedStatement@ pStmt = m_pConnection.CreatePreparedStatement( 
		"INSERT INTO Test (ID, VALUE, STRING, date, time, datetime) "
		"VALUES(?, ?, ?, ?, ?, ?), (?, ?, ?, ?, ?, ?)" );
		
		Print( "Created statement: %1\n", pStmt !is null ? "yes" : "no" );
		
		if( pStmt !is null )
		{
			for( int iIndex = 0; iIndex < 2; ++iIndex )
			{
				Print( "Index: %1\n", iIndex + 1 );
				pStmt.BindSigned32( ( iIndex * 6 ) + 0, iIndex + 1 );
				pStmt.BindSigned32( ( iIndex * 6 ) + 1, 10 );
				pStmt.BindString( ( iIndex * 6 ) + 2, "Foo" );
				pStmt.BindDate( ( iIndex * 6 ) + 3, DateTime::Now().GetDate() );
				pStmt.BindTime( ( iIndex * 6 ) + 4, Time::Now() );
				pStmt.BindDateTime( ( iIndex * 6 ) + 5, DateTime::Now() );
			}
			
			pStmt.ExecuteStatement( MySQLResultSetCallback( this.NullResultSetCallback ), MySQLPreparedStatementCallback( this.InsertedValues ) );
		}
	}
	
	private void NullResultSetCallback( MySQLResultSet@ pSet )
	{
		Print( "null result set callback\n" );
	}
	
	private void InsertedValues( MySQLPreparedStatement@ pStmt )
	{
		Print( "Inserted values\n" );
		
		/*
		MySQLPreparedStatement@ pStmt2 = m_pConnection.CreatePreparedStatement( "INSERT INTO Test (ID, VALUE, STRING, date, time, datetime) VALUES(1, ?, ?, ?, ?, ?)" );
		
		Print( "Created statement: %1\n", pStmt2 !is null ? "yes" : "no" );
		
		if( pStmt2 !is null )
		{
			pStmt2.BindSigned32( 0, 10 );
			pStmt2.BindString( 1, "Foo" );
			pStmt2.BindDate( 2, DateTime::Now().GetDate() );
			pStmt2.BindTime( 3, Time::Now() );
			pStmt2.BindDateTime( 4, DateTime::Now() );
			
			pStmt2.ExecuteStatement( MySQLResultSetCallback( this.NullResultSetCallback ) );
		}
		*/
		
		
		MySQLPreparedStatement@ pStmt2 = m_pConnection.CreatePreparedStatement( "SELECT * FROM Test" );
		
		if( pStmt2 !is null )
		{
			pStmt2.ExecuteStatement( MySQLResultSetCallback( this.Stmt2Callback ) );
		}
		
	}
	
	private void Stmt2Callback( MySQLResultSet@ pResultSet )
	{
		Print( "Statement 2 row callback invoked\n" );
		
		while( pResultSet.Next() )
		{
			Print( "Key: %1, Value: %2, String: %3\n", pResultSet.GetSigned32( 0 ), pResultSet.GetSigned32( 1 ), pResultSet.GetString( 2 ) );
		}
	}
}

Database g_Database;

void main()
{	
	g_Database.Connect();
}