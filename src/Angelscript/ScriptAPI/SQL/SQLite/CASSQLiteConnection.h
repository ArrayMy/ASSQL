#ifndef ANGELSCRIPT_SCRIPTAPI_SQL_SQLITE_CASSQLITECONNECTION_H
#define ANGELSCRIPT_SCRIPTAPI_SQL_SQLITE_CASSQLITECONNECTION_H

#include <sqlite3.h>

#include <Angelscript/util/CASBaseClass.h>

#include "../IASSQLConnection.h"

#include "CASSQLitePreparedStatement.h"

class CASSQLThreadPool;

/**
*	Angelscript SQLite connection.
*/
class CASSQLiteConnection : public IASSQLConnection, public CASAtomicRefCountedBaseClass
{
public:
	CASSQLiteConnection( const char* const pszFilename, CASSQLThreadPool& pool );

	/**
	*	Destructor.
	*	Closes the connection if it is still open.
	*/
	~CASSQLiteConnection();

	void AddRef() const override final
	{
		CASAtomicRefCountedBaseClass::AddRef();
	}

	void Release() const override final
	{
		if( InternalRelease() )
			delete this;
	}

	bool IsOpen() const override;

	void Close() override;

	bool Query( const std::string& szQuery, asIScriptFunction* const pCallback ) override;

	CASSQLitePreparedStatement* CreatePreparedStatement( const std::string& szStatement ) override;

	sqlite3* GetConnection() { return m_pConnection; }

	CASSQLThreadPool& GetThreadPool() { return m_ThreadPool; }

private:
	sqlite3* m_pConnection = nullptr;

	CASSQLThreadPool& m_ThreadPool;

private:
	CASSQLiteConnection( const CASSQLiteConnection& ) = delete;
	CASSQLiteConnection& operator=( const CASSQLiteConnection& ) = delete;
};

#endif //ANGELSCRIPT_SCRIPTAPI_SQL_SQLITE_CASSQLITECONNECTION_H