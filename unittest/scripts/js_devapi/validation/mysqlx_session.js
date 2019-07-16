//@ Session: accessing Schemas
|<Schema:mysql>|
|<Schema:information_schema>|

//@ Session: accessing individual schema
|mysql|
|information_schema|

//@ Session: accessing unexisting schema
||Unknown database 'unexisting_schema'

//@ Session: current schema validations: nodefault
|null|
|null|

//@ Session: create schema success
|<Schema:node_session_schema>|

//@ Session: create schema failure
||Can't create database 'node_session_schema'; database exists

//@ Session: Transaction handling: rollback
|Inserted Documents: 0|

//@ Session: Transaction handling: commit
|Inserted Documents: 3|



//@ Transaction Savepoints Initialization
||

//@# Savepoint Error Conditions (WL10869-ET1_2)
||Session.setSavepoint: Argument #1 is expected to be a string
||Session.setSavepoint: Invalid number of arguments, expected 0 to 1 but got 2

//@ Create a savepoint without specifying a name (WL10869-SR1_1)
|Autogenerated Savepoint: TXSP1|

//@<OUT> WL10869-SR1_1: Documents
{
    "_id": "5C514FF38144B714E7119BCF48B4CA98",
    "name": "test1"
}
{
    "_id": "5C514FF38144B714E7119BCF48B4CA99",
    "name": "test2"
}

//@<OUT> Rollback a savepoint using a savepoint with auto generated name (WL10869-SR1_2)
{
    "_id": "5C514FF38144B714E7119BCF48B4CA98",
    "name": "test1"
}


//@ Release a savepoint using a savepoint with auto generated name (WL10869-SR1_3)
|Autogenerated Savepoint: TXSP2|

//@ Create multiple savepoints with auto generated name and verify are unique in the session (WL10869-SR1_4)
|Autogenerated Savepoint: TXSP3|

//@<OUT> WL10869-SR1_4: Documents
{
    "_id": "5C514FF38144B714E7119BCF48B4CA98",
    "name": "test1"
}
{
    "_id": "5C514FF38144B714E7119BCF48B4CBA0",
    "name": "test3"
}


//@ Create a savepoint specifying a name (WL10869-SR1_5)
|Savepoint: mySavedPoint|

//@<OUT> WL10869-SR1_5: Documents
{
    "_id": "5C514FF38144B714E7119BCF48B4CA98",
    "name": "test1"
}
{
    "_id": "5C514FF38144B714E7119BCF48B4CBA0",
    "name": "test3"
}
{
    "_id": "5C514FF38144B714E7119BCF48B4CBA1",
    "name": "test4"
}


//@<OUT> Rollback a savepoint using a savepoint created with a custom name (WL10869-SR1_6)
{
    "_id": "5C514FF38144B714E7119BCF48B4CA98",
    "name": "test1"
}
{
    "_id": "5C514FF38144B714E7119BCF48B4CBA0",
    "name": "test3"
}


//@ Release a savepoint using a savepoint with a custom name (WL10869-SR1_6)
|Savepoint: anotherSP|

//@<OUT> Create a savepoint several times with the same name, the savepoint must be overwritten (WL10869-ET1_3)
{
    "_id": "5C514FF38144B714E7119BCF48B4CBA1",
    "name": "test4"
}
{
    "_id": "5C514FF38144B714E7119BCF48B4CBA2",
    "name": "test5"
}

//@ Releasing the savepoint
||

//@ Release the same savepoint multiple times, an error must be thrown (WL10869-ET1_4)
||SAVEPOINT anotherSP does not exist

//@ Rollback a non existing savepoint, exception must be thrown (WL10869-ET1_7)
||SAVEPOINT unexisting does not exist

//@ Final rollback
|Empty set|

//@<OUT> Rollback and Release a savepoint after a transaction commit, error must be thrown
{
    "_id": "5C514FF38144B714E7119BCF48B4CBA4",
    "name": "test7"
}

//@ rollbackTo after commit (WL10869-ET1_8)
||SAVEPOINT TXSP4 does not exist

//@ release after commit (WL10869-ET1_8)
||SAVEPOINT TXSP4 does not exist

//@<OUT> Rollback and Release a savepoint after a transaction rollback, error must be thrown
{
    "_id": "5C514FF38144B714E7119BCF48B4CBA4",
    "name": "test7"
}

//@ rollbackTo after rollback (WL10869-ET1_9)
||SAVEPOINT TXSP5 does not exist

//@ release after rollback (WL10869-ET1_9)
||SAVEPOINT TXSP5 does not exist


//@ Session: test for drop schema functions
||Invalid object member dropCollection (AttributeError)
||Invalid object member dropTable (AttributeError)
||Invalid object member dropView (AttributeError)


//@ Session: Testing dropping existing schema
|undefined|

//@ Session: Testing if the schema is actually dropped
||Unknown database 'node_session_schema'

//@ Session: Testing dropping non-existing schema
|undefined|


//@ Session: current schema validations: nodefault, mysql
|null|
|<Schema:mysql>|

//@ Session: current schema validations: nodefault, information_schema
|null|
|<Schema:information_schema>|

//@ Session: current schema validations: default
|<Schema:mysql>|
|<Schema:mysql>|

//@ Session: current schema validations: default, information_schema
|<Schema:mysql>|
|<Schema:information_schema>|

//@ Session: setFetchWarnings(false)
|0|

//@ Session: setFetchWarnings(true)
|1|
|Can't drop database 'unexisting'; database doesn't exist|

//@ Session: quoteName no parameters
||Session.quoteName: Invalid number of arguments, expected 1 but got 0

//@ Session: quoteName wrong param type
||Argument #1 is expected to be a string

//@ Session: quoteName with correct parameters
|`sample`|
|`sam``ple`|
|```sample```|
|```sample`|
|`sample```|

//@# Session: bad params
||Invalid connection options, expected either a URI or a Dictionary.
||Invalid connection options, expected either a URI or a Dictionary.
||Invalid connection options, expected either a URI or a Dictionary.
||Invalid connection options, expected either a URI or a Dictionary.
