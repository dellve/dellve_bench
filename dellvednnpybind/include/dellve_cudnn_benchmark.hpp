#ifndef DELLVE_CUDNN_BENCHMARK_HPP
#define DELLVE_CUDNN_BENCHMARK_HPP

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <cuda.h>
#include <cuda_device_runtime_api.h>
#include <cudnn.h>

#include "CuDNN/Status.hpp"

#include <iostream>
#include <functional>
#include <thread>
#include <tuple>

#include <chrono>

namespace DELLve {
    namespace Functional {
        template<int ...>
        struct seq {};
        template<int N, int ...S>
        struct gens : gens<N-1, N-1, S...> {};
        template<int ...S>
        struct gens<0, S...>{ typedef seq<S...> type; };
        
        template <typename T, typename ...Args>
        class delayed {
            std::tuple<Args...> args_;
            T (*func_)(Args...);
        public:
            delayed(T (*f)(Args...), std::tuple<Args...> a) :
                args_(a), func_(f) {};
            T call() const {
                return callWithArgs(typename gens<sizeof...(Args)>::type());
            }
        private:
            template<int ...S>
            T callWithArgs(seq<S...>) const {
                return func_(std::get<S>(args_)...);
            }
        };
    }
    
    typedef std::function<CuDNN::Status(void)> Benchmark;

    class BenchmarkController {
       
        int numRuns_;

        volatile bool stop_;
        volatile int currRun_;
        volatile int currTimeMicro_;
        
    public:
        
        void start ( int deviceId, int numRuns ) {
            numRuns_ = numRuns;
            stop_ = false;
            currRun_ = -1;
            currTimeMicro_ = 0;

            std::thread([=](){
                cudaSetDevice(deviceId);
                cudaDeviceSynchronize();
                Benchmark benchmark = getBenchmark();

                for (currRun_ = 0; currRun_ < numRuns && !stop_; currRun_++) {
                    auto start = std::chrono::steady_clock::now();

                    CuDNN::checkStatus(benchmark());
                    cudaDeviceSynchronize();

                    auto end = std::chrono::steady_clock::now();
                    currTimeMicro_ += static_cast<int>(std::chrono::duration<double, std::micro>(end-start).count());
                }
            }).detach();
        }

        void stop() {
            stop_ = true;
        }

        int getCurrRun() const {
            return currRun_;
        }

        float getProgress() const {
            return (float) (currRun_)/numRuns_;
        }

        int getCurrTimeMicro() const {
            return currTimeMicro_;
        }

        int getAvgTimeMicro() const {
            return currTimeMicro_/currRun_;
        }
        
    private:
        
        virtual Benchmark getBenchmark() = 0;
        
    };
    
    template <typename ... A>
    class BenchmarkDriver : public BenchmarkController {
        
        std::tuple<A...> args_;
        Benchmark (*func_)(A...);
    
    public:
        
        BenchmarkDriver(Benchmark (*func)(A...), A ... args) :
            args_(std::make_tuple(args...)),
            func_(func) {}
        
    private:
    
        Benchmark getBenchmark() {
            return Functional::delayed<Benchmark, A...>(func_, args_).call();
        }
        
    };

	template <typename T, typename ... A>
	void registerBenchmark (T module, const char* benchmarkName, Benchmark (*factory)(A...) )
	{
        module.def(benchmarkName, [=](A ... args) {
            return std::unique_ptr<BenchmarkController>(new BenchmarkDriver<A...>(factory, args...));
		});
	}
}

#endif // DELLVE_CUDNN_BENCHMARK_HPP
