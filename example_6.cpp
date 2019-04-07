#include <iostream>
#include <mutex>
#include <shared_mutex>

#define STDX_USING_ALL
#include "utility.hpp"
#include "mutex.hpp"
#include "stopwatch.hpp"

int example_6()
{
	stdx::desync_IO();

	STDX_DEFINE_STOPWATCH(stdx::chrono::nanoseconds);

	std::mutex mtx;
	std::shared_mutex sh_mtx;
	stdx::spin_mutex sp_mtx;
	stdx::shared_spin_mutex sh_sp_mtx;
	stopwatch::rep_type elapsed;
	stopwatch::rep_type sp_elapsed;

	// TODO test these with multiple threads

	// ==============================
	//             Mutexes
	// ==============================

	stopwatch::start();
	mtx.lock();
	elapsed = stopwatch::stop();

	std::cout <<
		"Elapsed time locking std::mutex: " << elapsed
	<< std::endl;

	stopwatch::start();
	sp_mtx.lock();
	sp_elapsed = stopwatch::stop();

	std::cout << 
		"Elapsed time locking stdx::spin_mutex: " << sp_elapsed
	<< std::endl;

	std::cout <<
		(elapsed < sp_elapsed ? "std::mutex" : (elapsed > sp_elapsed ? "stdx::spin_mutex" : "std::mutex and stdx::spin_mutex")) << (elapsed == sp_elapsed ? " are equal\n" : " is faster\n")
	<< std::endl;

	stopwatch::start();
	mtx.unlock();
	elapsed = stopwatch::stop();

	std::cout <<
		"Elapsed time unlocking std::mutex: " << elapsed
	<< std::endl;

	stopwatch::start();
	sp_mtx.unlock();
	sp_elapsed = stopwatch::stop();

	std::cout <<
		"Elapsed time unlocking stdx::spin_mutex: " << sp_elapsed
	<< std::endl;

	std::cout <<
		(elapsed < sp_elapsed ? "std::mutex" : (elapsed > sp_elapsed ? "stdx::spin_mutex" : "std::mutex and stdx::spin_mutex")) << (elapsed == sp_elapsed ? " are equal\n" : " is faster\n")
	<< std::endl;

	// ==============================
	//   Shared mutexes (exclusive)
	// ==============================

	stopwatch::start();
	sh_mtx.lock();
	elapsed = stopwatch::stop();

	std::cout <<
		"Elapsed time locking (exclusive) std::shared_mutex: " << elapsed
	<< std::endl;

	stopwatch::start();
	sh_sp_mtx.lock();
	sp_elapsed = stopwatch::stop();

	std::cout <<
		"Elapsed time locking (exclusive) stdx::shared_spin_mutex: " << sp_elapsed
	<< std::endl;

	std::cout << 
		(elapsed < sp_elapsed ? "std::shared_mutex" : (elapsed > sp_elapsed ? "stdx::shared_spin_mutex" : "std::shared_mutex and stdx::shared_spin_mutex")) << (elapsed == sp_elapsed ? " are equal\n" : " is faster\n")
	<< std::endl;

	stopwatch::start();
	sh_mtx.unlock();
	elapsed = stopwatch::stop();

	std::cout <<
		"Elapsed time unlocking (exclusive) std::shared_mutex: " << elapsed
	<< std::endl;

	stopwatch::start();
	sh_sp_mtx.unlock();
	sp_elapsed = stopwatch::stop();

	std::cout <<
		"Elapsed time unlocking (exclusive) stdx::shared_spin_mutex: " << sp_elapsed
	<< std::endl;

	std::cout << 
		(elapsed < sp_elapsed ? "std::shared_mutex" : (elapsed > sp_elapsed ? "stdx::shared_spin_mutex" : "std::shared_mutex and stdx::shared_spin_mutex")) << (elapsed == sp_elapsed ? " are equal\n" : " is faster\n")
	<< std::endl;

	// ==============================
	//     Shared mutexes (shared)
	// ==============================

	stopwatch::start();
	sh_mtx.lock_shared();
	elapsed = stopwatch::stop();

	std::cout <<
		"Elapsed time locking (shared) std::shared_mutex: " << elapsed
	<< std::endl;

	stopwatch::start();
	sh_sp_mtx.lock_shared();
	sp_elapsed = stopwatch::stop();

	std::cout <<
		"Elapsed time locking (shared) stdx::shared_spin_mutex: " << sp_elapsed
	<< std::endl;

	std::cout <<
		(elapsed < sp_elapsed ? "std::shared_mutex" : (elapsed > sp_elapsed ? "stdx::shared_spin_mutex" : "std::shared_mutex and stdx::shared_spin_mutex")) << (elapsed == sp_elapsed ? " are equal\n" : " is faster\n")
	<< std::endl;

	stopwatch::start();
	sh_mtx.unlock_shared();
	elapsed = stopwatch::stop();

	std::cout <<
		"Elapsed time unlocking (shared) std::shared_mutex: " << elapsed
	<< std::endl;

	stopwatch::start();
	sh_sp_mtx.unlock_shared();
	sp_elapsed = stopwatch::stop();

	std::cout <<
		"Elapsed time unlocking (shared) stdx::shared_spin_mutex: " << sp_elapsed
	<< std::endl;

	std::cout <<
		(elapsed < sp_elapsed ? "std::shared_mutex" : (elapsed > sp_elapsed ? "stdx::shared_spin_mutex" : "std::shared_mutex and stdx::shared_spin_mutex")) << (elapsed == sp_elapsed ? " are equal\n" : " is faster\n")
	<< std::endl;

	stdx::await_input();

	return 0;
}
