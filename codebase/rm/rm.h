
#ifndef _rm_h_
#define _rm_h_

#include <string>
#include <vector>

#include "../rbf/rbfm.h"
#include "../ix/ix.h"

using namespace std;

# define RM_EOF (-1)  // end of a scan operator

typedef pair<pair<int, RID>, pair<string, string> > IndexDataSchema;

// RM_ScanIterator is an iteratr to go through tuples
class RM_ScanIterator {
public:
	RBFM_ScanIterator rbfmScanIterator;

  RM_ScanIterator() {};
  ~RM_ScanIterator() {};

  // "data" follows the same format as RelationManager::insertTuple()
  RC getNextTuple(RID &rid, void *data);
  RC close();
};


// RM_IndexScanIterator is an iterator to go through index entries
class RM_IndexScanIterator {
 public:
	IX_ScanIterator ixScanIterator;
	IndexManager *indexManager;
  RM_IndexScanIterator() {
	  indexManager = IndexManager::instance();
  };  	// Constructor
  ~RM_IndexScanIterator() {}; 	// Destructor

  // "key" follows the same format as in IndexManager::insertEntry()
  RC getNextEntry(RID &rid, void *key);  	// Get next matching entry
  RC close();             			// Terminate index scan
};

// Relation Manager
class RelationManager
{
public:
	RecordBasedFileManager *rbfm;
	IndexManager *indexManager;
	const string TABLES_FILE = "Tables";
	const string COLUMNS_FILE = "Columns";
	const string TABLES_TABLE_NAME = "Tables";
	const string COLUMNS_TABLE_NAME = "Columns";
	const string INDEX_TABLE_FILENAME = "IndexTable";
	const string INDEX_TABLE_NAME = "Index";
	vector<Attribute> tablesAttributes;
	vector<Attribute> columnsAttributes;
	vector<Attribute> indexAttributes;
	int SYSTEM_TABLE = 1;
	int USER_TABLE = 0;
	int TABLES_TABLE_ID =  1;
	int COLUMNS_TABLE_ID = 2;
	int INDEX_TABLE_ID = 3;
	int nextTableId = 4;


  static RelationManager* instance();

  RC createCatalog();

  RC deleteCatalog();

  RC createTable(const string &tableName, const vector<Attribute> &attrs);

  RC deleteTable(const string &tableName);

  RC getAttributes(const string &tableName, vector<Attribute> &attrs);

  RC insertTuple(const string &tableName, const void *data, RID &rid);

  RC deleteTuple(const string &tableName, const RID &rid);

  RC updateTuple(const string &tableName, const void *data, const RID &rid);

  RC readTuple(const string &tableName, const RID &rid, void *data);

  // Print a tuple that is passed to this utility method.
  // The format is the same as printRecord().
  RC printTuple(const vector<Attribute> &attrs, const void *data);

  RC readAttribute(const string &tableName, const RID &rid, const string &attributeName, void *data);

  // Scan returns an iterator to allow the caller to go through the results one by one.
  // Do not store entire results in the scan iterator.
  RC scan(const string &tableName,
      const string &conditionAttribute,
      const CompOp compOp,                  // comparison type such as "<" and "="
      const void *value,                    // used in the comparison
      const vector<string> &attributeNames, // a list of projected attributes
      RM_ScanIterator &rm_ScanIterator);
  RC createIndex(const string &tableName, const string &attributeName);

  RC destroyIndex(const string &tableName, const string &attributeName);

	// indexScan returns an iterator to allow the caller to go through qualified entries in index
  RC indexScan(const string &tableName,
						  const string &attributeName,
						  const void *lowKey,
						  const void *highKey,
						  bool lowKeyInclusive,
						  bool highKeyInclusive,
						  RM_IndexScanIterator &rm_IndexScanIterator);

  RC populateTablesAttributes();

  RC populateColumnsAttributes();

  RC createRecordInTablesTable(const void*data, int tableId, string tableName, string fileName, int systemTable);
  RC createRecordInColumnsTable(const void*data, int tableId, string columnName, AttrType columnType, int columnLength,
		  int columnPosition, int systemTable);
  RC getIDOfTable(const string &tableName, int &tabID, RID &rid, bool &systemTable, char *tableFileName);
  RC getNextValidTableID(int &nextValidTableID);

  RC getNextValidIndexID(int &nextValidIndexID);
  RC populateIndexAttributes();
  void deleteIndexEntriesForTable(int tableId);
  RC insertTupleIntoIndexIfExists(const string &tableName, int &tabId, const void *data, RID &rid, vector<Attribute> &attributes);
  void findIndexesForTable(int &tabId, vector<IndexDataSchema> &indexDetails);
  void deleteTupleFromIndexIfExists(int &tableId, vector<Attribute> &attributes, const void* recordData, RID &rid);
  void createRecordInIndexTable(void* data, int indexId, int tableId, const string &tableName, const string &attributeName,
		  AttrType attributeType, const string &fileName);

// Extra credit work (10 points)
public:
  RC addAttribute(const string &tableName, const Attribute &attr);

  RC dropAttribute(const string &tableName, const string &attributeName);


protected:
  RelationManager();
  ~RelationManager();

private:
  static RelationManager *_rm;
};

#endif
