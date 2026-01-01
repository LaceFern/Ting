#include "TransactionManager.h"
namespace Database {
  bool TransactionManager::InsertRecord(TxnContext* context, 
      size_t table_id, const IndexKey* keys, 
      size_t key_num, Record *record, const GAddr& data_addr) {
    RecordSchema *schema_ptr = storage_manager_->tables_[table_id]->GetSchema();
    bool lock_success = TryWLockRecord(data_addr, schema_ptr->GetSchemaSize());
    assert(lock_success == true); 
    record->SetVisible(true);
    Access* access = access_list_.NewAccess();
    access->access_type_ = INSERT_ONLY;
    access->access_record_ = record;
    access->access_addr_ = data_addr;
    return true;
  }
  bool TransactionManager::SelectRecordCC(
      TxnContext* context, size_t table_id, 
      Record *&record, const GAddr &data_addr, 
      AccessType access_type) {
    epicLog(LOG_DEBUG, "thread_id=%u,table_id=%u,access_type=%u,data_addr=%lx, start SelectRecordCC", 
        thread_id_, table_id, access_type, data_addr);
    RecordSchema *schema_ptr = storage_manager_->tables_[table_id]->GetSchema();
    bool lock_success = true;
    if (access_type == READ_ONLY) {
      lock_success = TryRLockRecord(data_addr, schema_ptr->GetSchemaSize());
      }
    else {
      lock_success = TryWLockRecord(data_addr, schema_ptr->GetSchemaSize());
      }
    if (lock_success) {
      record = new Record(schema_ptr);
      record->Deserialize(data_addr, gallocators[thread_id_]);
      Access* access = access_list_.NewAccess();
      access->access_type_ = access_type;
      access->access_record_ = record;
      access->access_addr_ = data_addr;
      if (access_type == DELETE_ONLY) {
        record->SetVisible(false);
      }
      return true;
    }
    else { 
      epicLog(LOG_DEBUG, "thread_id=%u,table_id=%u,access_type=%u,data_addr=%lx,lock fail, abort", 
          thread_id_, table_id, access_type,data_addr);
      this->AbortTransaction();
      return false;
    }
  }
  bool TransactionManager::CommitTransaction(TxnContext* context, 
      TxnParam* param, CharArray& ret_str) {
    epicLog(LOG_DEBUG, "thread_id=%u,txn_type=%d,commit", thread_id_, context->txn_type_);
    for (size_t i = 0; i < access_list_.access_count_; ++i) {
      Access* access = access_list_.GetAccess(i);
      assert(access->access_type_ == READ_ONLY || 
          access->access_type_ == DELETE_ONLY || 
          access->access_type_ == INSERT_ONLY || 
          access->access_type_ == READ_WRITE);
      Record *record = access->access_record_;
      if (access->access_type_ == READ_WRITE) {
        record->Serialize(access->access_addr_, gallocators[thread_id_]);
      }
      else if (access->access_type_ == DELETE_ONLY) {
        record->Serialize(access->access_addr_, 
            gallocators[thread_id_]);
      }
      this->UnLockRecord(access->access_addr_, record->GetSchemaSize());
    }
    for (size_t i = 0; i < access_list_.access_count_; ++i) {
      Access* access = access_list_.GetAccess(i);
      if (access->access_type_ == DELETE_ONLY) {
        gallocators[thread_id_]->Free(access->access_addr_);
        access->access_addr_ = DB_Gnullptr;
      }
      delete access->access_record_;
      access->access_record_ = nullptr;
      access->access_addr_ = DB_Gnullptr;
    }
    access_list_.Clear();
    return true;
  }
  void TransactionManager::AbortTransaction() {
    epicLog(LOG_DEBUG, "thread_id=%u,abort", thread_id_);
    for (size_t i = 0; i < access_list_.access_count_; ++i) {
      Access* access = access_list_.GetAccess(i);
      Record *record = access->access_record_;
      this->UnLockRecord(access->access_addr_, record->GetSchemaSize());
      if (access->access_type_ == INSERT_ONLY) {
        record->SetVisible(false);
        gallocators[thread_id_]->Free(access->access_addr_);
      }
    }
    for (size_t i = 0; i < access_list_.access_count_; ++i) {
      Access* access = access_list_.GetAccess(i);
      delete access->access_record_;
      access->access_record_ = nullptr;
      access->access_addr_ = DB_Gnullptr;
    }
    access_list_.Clear();
    }
}
