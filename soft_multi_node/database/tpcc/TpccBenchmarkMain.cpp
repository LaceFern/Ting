#include "TpccExecutor.h"
#include "TpccPopulator.h"
#include "TpccSource.h"
#include "TpccInitiator.h"
#include "TpccConstants.h"
#include "Meta.h"
#include "TpccParams.h"
#include "BenchmarkArguments.h"
#include "ClusterHelper.h"
#include "ClusterSync.h"
#include <iostream>

#include <algorithm>

std::vector<double> latency;
std::atomic<int> latency_lock{0};

using namespace Database::TpccBenchmark;
using namespace Database;

int main(int argc, char* argv[]) {
  ArgumentsParser(argc, argv);

  std::string my_host_name = ClusterHelper::GetLocalHostName();
  ClusterConfig config(my_host_name, port, config_filename);
  ClusterSync synchronizer(&config);
  FillScaleParams(config);
  PrintScaleParams();

  TpccInitiator initiator(gThreadCount, &config);
  initiator.InitGAllocator();

  default_gallocator->registerThread();

  synchronizer.Fence();
  GAddr storage_addr = initiator.InitStorage();
  synchronizer.MasterBroadcast<GAddr>(&storage_addr); 
  std::cout << "storage_addr=" << "node:" << storage_addr.nodeID << "addr:" <<storage_addr.addr << std::endl;
  StorageManager storage_manager;
  storage_manager.Deserialize(storage_addr, default_gallocator);

  TpccPopulator populator(&storage_manager, &tpcc_scale_params);
  populator.Start();
  synchronizer.Fence();

  IORedirector redirector(gThreadCount);
  size_t access_pattern = 0;
  TpccSource sourcer(&tpcc_scale_params, &redirector, num_txn,
                     SourceType::PARTITION_SOURCE, gThreadCount, dist_ratio,
                     config.GetMyPartitionId());
  sourcer.Start();
  synchronizer.Fence();

  {
    TpccExecutor executor(&redirector, &storage_manager, gThreadCount);
    executor.Start();
  }
  synchronizer.Fence();

  latency_lock.store(0);
  synchronizer.Fence();

  {
    TpccExecutor executor(&redirector, &storage_manager, gThreadCount);
    executor.Start();
  }

  synchronizer.Fence();
  return 0;
}