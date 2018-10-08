#include <iostream>
#include <mutex>
#include <shared_mutex>

#define STDX_USING_ALL
#include "utility.hpp"
#include "mutex.hpp"
#include "stopwatch.hpp"

int test6()
{
	stdx::unsynced_ios();

	std::mutex mtx;
	std::shared_mutex sh_mtx;
	stdx::spin_mutex sp_mtx;
	stdx::shared_spin_mutex sh_sp_mtx;
	double split = 0.0;
	double sp_split = 0.0;

	// ==============================
	//             Mutexes
	// ==============================

	stdx::stopwatch::start();
	mtx.lock();
	split = stdx::stopwatch::split();

	std::cout <<
		"Elapsed time locking std::mutex: " << split
	<< std::endl;

	stdx::stopwatch::split();
	sp_mtx.lock();
	sp_split = stdx::stopwatch::split();

	std::cout << 
		"Elapsed time locking stdx::spin_mutex: " << sp_split
	<< std::endl;

	std::cout << (split < sp_split ? "std::mutex" : (split > sp_split ? "stdx::spin_mutex" : "std::mutex and stdx::spin_mutex")) << (split == sp_split ? " are equal\n" : " is faster\n") << std::endl;

	stdx::stopwatch::split();
	mtx.unlock();
	split = stdx::stopwatch::split();

	std::cout <<
		"Elapsed time unlocking std::mutex: " << split
	<< std::endl;

	stdx::stopwatch::split();
	sp_mtx.unlock();
	sp_split = stdx::stopwatch::split();

	std::cout <<
		"Elapsed time unlocking stdx::spin_mutex: " << sp_split
	<< std::endl;

	std::cout << (split < sp_split ? "std::mutex" : (split > sp_split ? "stdx::spin_mutex" : "std::mutex and stdx::spin_mutex")) << (split == sp_split ? " are equal\n" : " is faster\n") << std::endl;

	// ==============================
	//   Shared mutexes (exclusive)
	// ==============================

	stdx::stopwatch::split();
	sh_mtx.lock();
	split = stdx::stopwatch::split();

	std::cout <<
		"Elapsed time locking (exclusive) std::shared_mutex: " << split
	<< std::endl;

	stdx::stopwatch::split();
	sh_sp_mtx.lock();
	sp_split = stdx::stopwatch::split();

	std::cout <<
		"Elapsed time locking (exclusive) stdx::shared_spin_mutex: " << sp_split
	<< std::endl;

	std::cout << (split < sp_split ? "std::shared_mutex" : (split > sp_split ? "stdx::shared_spin_mutex" : "std::shared_mutex and stdx::shared_spin_mutex")) << (split == sp_split ? " are equal\n" : " is faster\n") << std::endl;

	stdx::stopwatch::split();
	sh_mtx.unlock();
	split = stdx::stopwatch::split();

	std::cout <<
		"Elapsed time unlocking (exclusive) std::shared_mutex: " << split
	<< std::endl;

	stdx::stopwatch::split();
	sh_sp_mtx.unlock();
	sp_split = stdx::stopwatch::split();

	std::cout <<
		"Elapsed time unlocking (exclusive) stdx::shared_spin_mutex: " << sp_split
	<< std::endl;

	std::cout << (split < sp_split ? "std::shared_mutex" : (split > sp_split ? "stdx::shared_spin_mutex" : "std::shared_mutex and stdx::shared_spin_mutex")) << (split == sp_split ? " are equal\n" : " is faster\n") << std::endl;

	// ==============================
	//     Shared mutexes (shared)
	// ==============================

	stdx::stopwatch::split();
	sh_mtx.lock_shared();
	split = stdx::stopwatch::split();

	std::cout <<
		"Elapsed time locking (shared) std::shared_mutex: " << split
	<< std::endl;

	stdx::stopwatch::split();
	sh_sp_mtx.lock_shared();
	sp_split = stdx::stopwatch::split();

	std::cout <<
		"Elapsed time locking (shared) stdx::shared_spin_mutex: " << sp_split
	<< std::endl;

	std::cout << (split < sp_split ? "std::shared_mutex" : (split > sp_split ? "stdx::shared_spin_mutex" : "std::shared_mutex and stdx::shared_spin_mutex")) << (split == sp_split ? " are equal\n" : " is faster\n") << std::endl;

	stdx::stopwatch::split();
	sh_mtx.unlock_shared();
	split = stdx::stopwatch::split();

	std::cout <<
		"Elapsed time unlocking (shared) std::shared_mutex: " << split
	<< std::endl;

	stdx::stopwatch::split();
	sh_sp_mtx.unlock_shared();
	sp_split = stdx::stopwatch::stop();

	std::cout <<
		"Elapsed time unlocking (shared) stdx::shared_spin_mutex: " << sp_split
	<< std::endl;

	std::cout << (split < sp_split ? "std::shared_mutex" : (split > sp_split ? "stdx::shared_spin_mutex" : "std::shared_mutex and stdx::shared_spin_mutex")) << (split == sp_split ? " are equal\n" : " is faster\n") << std::endl;

	stdx::ignoreline();

	return 0;
}
