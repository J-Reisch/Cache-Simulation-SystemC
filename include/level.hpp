//
// Created by julian on 1/3/25.
//

#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <systemc>
#include <cstdint>
#include <cmath>
#include <unordered_map>

#include "cache_set.hpp"

using namespace sc_core;

SC_MODULE(LEVEL) {
  	sc_in<bool> clk;
	sc_in<uint32_t> addr;
	sc_in<uint32_t> wdata;
    sc_in<bool> w;
    sc_in<bool> r;
	sc_in<bool> access;

	sc_out<uint32_t> rdata;
	sc_out<bool> ready;
	sc_out<bool> miss;

	uint32_t cacheLineSize;
	uint32_t numLines;
	uint32_t latency;
	uint8_t mappingStrategy;
	uint32_t numLinesPerSet;

	uint32_t numCacheSets;
	uint8_t numOffsetBits;
	uint8_t numIndexBits;
	uint8_t numTagBits;

	std::unordered_map<uint32_t, CacheSet> cacheSets;

    SC_HAS_PROCESS(LEVEL);
    LEVEL(sc_module_name name, uint32_t cacheLineSize, uint32_t numLines, uint32_t latency, uint8_t mappingStrategy, uint32_t numLinesPerSet);

	void printLevel();

    void behaviour();

	uint8_t getCacheLineContentOfLevel(uint32_t address);
};

#endif //LEVEL_HPP
