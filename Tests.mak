#
#    Copyright (C) 2011-2013,2018 Alexey V. Medvedev
#
#    This file is part of cppcgen. cppcgen is a C++-11 generator of C code with macroses. 
#
#    cppcgen is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    cppcgen is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with cppcgen.  If not, see <http://www.gnu.org/licenses/>.
#


CPPCGEN_LIB_DIR = distr/lib
CPPCGEN_INCLUDE_DIR = distr/include
CPPCGEN = libcppcgen.so
CPPCGEN_SHORT = cppcgen

HDR_FILES = $(wildcard $(CPPCGEN_INCLUDE_DIR)/*.h)
CPPCGEN_FILE = $(wildcard $(CPPCGEN_LIB_DIR)/$(CPPCGEN))

override CPPFLAGS += -I$(CPPCGEN_INCLUDE_DIR)
override CXXFLAGS += -std=c++11 -Wall -Wextra -pedantic -O3

TEST1_BIN = test/test1
TEST1 = test/test1.cpp
TEST1_OBJ = $(subst .cpp,.o,$(TEST1))

TEST2_BIN = test/test2
TEST2 = test/test2.cpp
TEST2_OBJ = $(subst .cpp,.o,$(TEST2))

TEST3_BIN = test/test3
TEST3 = test/test3.cpp
TEST3_OBJ = $(subst .cpp,.o,$(TEST3))

TEST4_BIN = test/test4
TEST4 = test/test4.cpp
TEST4_OBJ = $(subst .cpp,.o,$(TEST4))

TEST5_BIN = test/test5
TEST5 = test/test5.cpp
TEST5_OBJ = $(subst .cpp,.o,$(TEST5))

TEST6_BIN = test/test6
TEST6 = test/test6.cpp
TEST6_OBJ = $(subst .cpp,.o,$(TEST6))

TEST7_BIN = test/test7
TEST7 = test/test7.cpp
TEST7_OBJ = $(subst .cpp,.o,$(TEST7))

TEST_BIN = $(TEST1_BIN) $(TEST2_BIN) $(TEST3_BIN) $(TEST4_BIN) $(TEST5_BIN) $(TEST6_BIN) $(TEST7_BIN)
TEST = $(TEST1) $(TEST2) $(TEST3) $(TEST4) $(TEST5) $(TEST6) $(TEST7)
TEST_OBJ = $(TEST1_OBJ) $(TEST2_OBJ) $(TEST3_OBJ) $(TEST4_OBJ) $(TEST5_OBJ) $(TEST6_OBJ) $(TEST7_OBJ)

.PHONY: all sandbox clean showfiles distclean

all: sandbox

sandbox: $(TEST_BIN)
	@mkdir -p sandbox
	@cp $(CPPCGEN_FILE) $(TEST_BIN) test/test7_stub.cpp  sandbox

$(TEST1_BIN): $(TEST1_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ -L$(CPPCGEN_LIB_DIR) -l$(CPPCGEN_SHORT) -Wl,-rpath=.
	
$(TEST2_BIN): $(TEST2_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ -L$(CPPCGEN_LIB_DIR) -l$(CPPCGEN_SHORT) -Wl,-rpath=.

$(TEST3_BIN): $(TEST3_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ -L$(CPPCGEN_LIB_DIR) -l$(CPPCGEN_SHORT) -Wl,-rpath=.
	
$(TEST4_BIN): $(TEST4_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ -L$(CPPCGEN_LIB_DIR) -l$(CPPCGEN_SHORT) -Wl,-rpath=.

$(TEST5_BIN): $(TEST5_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ -L$(CPPCGEN_LIB_DIR) -l$(CPPCGEN_SHORT) -Wl,-rpath=.
	
$(TEST6_BIN): $(TEST6_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ -L$(CPPCGEN_LIB_DIR) -l$(CPPCGEN_SHORT) -Wl,-rpath=.

$(TEST7_BIN): $(TEST7_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ -L$(CPPCGEN_LIB_DIR) -l$(CPPCGEN_SHORT) -Wl,-rpath=.

$(TEST7_OBJ): test/test7_stub.cpp

clean:
	rm -f $(TEST_OBJ) $(TEST_BIN)

distclean: clean
	rm -rf sandbox
	find test -name '*~' | xargs rm -f

showfiles:
	@echo $(TEST)

$(TEST_OBJ): $(CPPCGEN_FILE) $(HDR_FILES) Tests.mak
