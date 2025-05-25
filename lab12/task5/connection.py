import psycopg2
from psycopg2.extras import RealDictCursor
from psycopg2 import sql


DB_PARAMS = {
    "dbname": "postgres",
    "user": "postgres",
    "password": "higz2006",
    "host": "localhost",
    "port": 5432
}

class PostgreSQLConnection:
    def __init__(self, params = DB_PARAMS):
        self.conn_params = params
        self.connection = None

    def connect(self):
        try:
            self.connection = psycopg2.connect(**self.conn_params)
        except Exception as ex:
            print(f"An error occured: {ex}")

    def disconnect(self):
        if self.connection:
            self.connection.close() 
        else:
            print("No connection established")

    def execute_query(self, query, params=None, explain=False):
        try:
            with self.connection.cursor(cursor_factory=RealDictCursor) as cursor:
                if explain:

                    explain_query = sql.SQL("EXPLAIN ANALYZE {}").format(sql.SQL(query))
                    cursor.execute(explain_query, params)
                    plan = cursor.fetchall()
                    return "\n".join(row['QUERY PLAN'] for row in plan)
                
                else:

                    cursor.execute(query, params)
                    if cursor.description:
                        return cursor.fetchall()
                    return None
                
        except Exception as ex:
            print(f"An error occured: {ex}")

    


if __name__ == "__main__":
    conn = PostgreSQLConnection()

    conn.connect()

    with open("scripts/less_work_mem.sql", 'r') as file:
        work_mem_query = file.read()

    conn.execute_query(work_mem_query)

    with open("scripts/query.sql", 'r') as file:
        query = file.read()
    
    result = conn.execute_query(query, explain=True)

    print(result)

    with open("scripts/default_work_mem.sql", 'r') as file:
        work_mem_query = file.read()

    conn.execute_query(work_mem_query)


    with open("scripts/query.sql", 'r') as file:
        query = file.read()
    
    result = conn.execute_query(query, explain=True)


    print(result)



    