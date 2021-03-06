shell.connect(__uripwd);
session.drop_schema('prepared_stmt');
schema = session.create_schema('prepared_stmt');
collection = schema.create_collection('test_collection');

collection.add({'_id': '001', 'name': 'george', 'age': 18}).execute();
collection.add({'_id': '002', 'name': 'james', 'age': 17}).execute();
collection.add({'_id': '003', 'name': 'luke', 'age': 18}).execute();
collection.add({'_id': '004', 'name': 'mark', 'age': 21}).execute();

#@ sql() first execution is normal
sql = session.sql('select * from prepared_stmt.test_collection');
sql.execute();

#@ sql() Second execution attempts preparing statement, disables prepared statements on session, executes normal
sql.execute();

#@ Third execution does normal execution
sql.execute();

#@ find() first call
crud = collection.find();
crud.execute();

#@ find() second call, no prepare
crud.execute()

#@ remove() first call
crud = collection.remove('_id = :id');
crud.bind('id', '004').execute();
collection.find();

#@ remove() second call, no prepare
crud.bind('id', '003').execute();
collection.find();

#@ modify() first call
crud = collection.modify('_id = :id').set('age', 20);
crud.bind('id', '001').execute();
collection.find();

#@ modify() second call, no prepare
crud.bind('id', '002').execute();
collection.find();

#@<> Finalizing
session.drop_schema('prepared_stmt');
session.close();
