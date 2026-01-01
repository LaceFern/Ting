#ifndef __DATABASE_TPCC_CONSTANTS_H__
#define __DATABASE_TPCC_CONSTANTS_H__
#include <string>
namespace Database {
namespace TpccBenchmark {
enum TupleType
  : size_t {
    DELIVERY,
  NEW_ORDER,
  PAYMENT,
  ORDER_STATUS,
  STOCK_LEVEL,
  kProcedureCount
};
enum TableType
  : size_t {
    ITEM_TABLE_ID,
  STOCK_TABLE_ID,
  WAREHOUSE_TABLE_ID,
  DISTRICT_TABLE_ID,
  DISTRICT_NEW_ORDER_TABLE_ID,
  NEW_ORDER_TABLE_ID,
  ORDER_TABLE_ID,
  ORDER_LINE_TABLE_ID,
  CUSTOMER_TABLE_ID,
  HISTORY_TABLE_ID,
  kTableCount
};
const int FREQUENCY_DELIVERY = 0;  
const int FREQUENCY_PAYMENT = 43;
const int FREQUENCY_NEW_ORDER = 45;
const int FREQUENCY_ORDER_STATUS = 0;  
const int FREQUENCY_STOCK_LEVEL = 0;  
const size_t kWarehouseBits = 58;
const size_t kTableIdLowBits = 54;
const size_t kTableIdHighBits = 6;
const size_t kDistrictLowBits = 32;
const size_t kOrderIdLowBits = 4;
const int MONEY_DECIMALS = 2;
const int NUM_ITEMS = 100000;
const int MIN_IM = 1;
const int MAX_IM = 10000;
const double MIN_PRICE = 1.00;
const double MAX_PRICE = 100.00;
const int MIN_I_NAME = 14;
const int MAX_I_NAME = 24;
const int MIN_I_DATA = 26;
const int MAX_I_DATA = 50;
const double MIN_TAX = 0;
const double MAX_TAX = 0.2000;
const int TAX_DECIMALS = 4;
const double INITIAL_W_YTD = 300000.00;
const int MIN_NAME = 6;
const int MAX_NAME = 10;
const int MIN_STREET = 10;
const int MAX_STREET = 20;
const int MIN_CITY = 10;
const int MAX_CITY = 20;
const int STATE = 2;
const int ZIP_LENGTH = 9;
const std::string ZIP_SUFFIX = "11111";
const int MIN_QUANTITY = 10;
const int MAX_QUANTITY = 100;
const int DIST = 24;
const int STOCK_PER_WAREHOUSE = 100000;
const int DISTRICTS_PER_WAREHOUSE = 10;
const double INITIAL_D_YTD = 30000.00;  
const int CUSTOMERS_PER_DISTRICT = 3000;
const double INITIAL_CREDIT_LIM = 50000.00;
const double MIN_DISCOUNT = 0.0000;
const double MAX_DISCOUNT = 0.5000;
const int DISCOUNT_DECIMALS = 4;
const double INITIAL_BALANCE = -10.00;
const double INITIAL_YTD_PAYMENT = 10.00;
const int INITIAL_PAYMENT_CNT = 1;
const int INITIAL_DELIVERY_CNT = 0;
const int MIN_FIRST = 6;
const int MAX_FIRST = 10;
const std::string MIDDLE = "OE";
const int PHONE = 16;
const int MIN_C_DATA = 300;
const int MAX_C_DATA = 500;
const std::string GOOD_CREDIT = "GC";
const std::string BAD_CREDIT = "BC";
const int MIN_CARRIER_ID = 1;
const int MAX_CARRIER_ID = 10;
const int NULL_CARRIER_ID = 0;
const int NULL_CARRIER_LOWER_BOUND = 2101;
const int MIN_OL_CNT = 5;
const int MAX_OL_CNT = 15;
const bool INITIAL_ALL_LOCAL = true;
const int INITIAL_ORDERS_PER_DISTRICT = 3000;
const int MAX_OL_QUANTITY = 10;
const int INITIAL_QUANTITY = 5;
const double MIN_AMOUNT = 0.01;
const int MIN_DATA = 12;
const int MAX_DATA = 24;
const double INITIAL_AMOUNT = 10.00;
const int INITIAL_NEW_ORDERS_PER_DISTRICT = 900;
const std::string INVALID_ITEM_MESSAGE = "Item number is not valid";
const int MIN_STOCK_LEVEL_THRESHOLD = 10;
const int MAX_STOCK_LEVEL_THRESHOLD = 20;
const double MIN_PAYMENT = 1.0;
const double MAX_PAYMENT = 5000.0;
const std::string ORIGINAL_STRING = "ORIGINAL";
}
}
#endif
