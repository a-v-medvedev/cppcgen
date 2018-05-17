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

MAKEFLAGS += --no-print-directory

LIB = lib/libcppcgen.so

SRC = src/term.cpp src/expr.cpp src/exprdir.cpp src/basic_expr.cpp src/expression.cpp
OBJ = $(subst .cpp,.o,$(SRC))
HDR = src/cppcgen_basic_expr.h src/cppcgen_output.h src/cppcgen_exprdir.h src/cppcgen_helpers.h src/cppcgen_term.h src/cppcgen_pair.h src/cppcgen_expr.h src/cppcgen_expression.h src/cppcgen.h src/cppcgen_basic_syntax.h src/cppcgen_utils.h

override CPPFLAGS += -Isrc 
override CXXFLAGS += -std=c++11 -Wall -Wextra -pedantic -O3 -fPIC

.PHONY: all lib clean showfiles tests distr distclean

all: lib distr tests

lib: $(LIB)

$(LIB): $(OBJ)
	@mkdir -p lib
	$(CXX) -shared -o $@ $^

distr: $(LIB) $(HDR)
	@mkdir -p distr
	@mkdir -p distr/include distr/lib
	@cp -u $(LIB) distr/lib
	@cp -u $(HDR) distr/include

tests: distr
	@+make -f Tests.mak

clean:
	rm -f $(LIB) $(OBJ)
	make -f Tests.mak clean

distclean: clean
	rm -rf distr lib
	find src -name '*~' | xargs rm -f
	rm -f *~
	@make -f Tests.mak distclean
	@find . -name '*.o' | while read f; do echo "NOTE: $$f file has left after clean!"; done

showfiles:
	@echo $(SRC) $(TEST) $(HDR) `make -f Tests.mak showfiles`

$(OBJ): $(HDR) Makefile
