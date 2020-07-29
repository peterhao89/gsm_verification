XX=g++
FLAG=-O2 -std=c++11 -lgmp
merge:mainA51.cpp A51Impl.hpp GuessDetermineCP.hpp CommonUtils.hpp PracticalAttack.hpp
	$(XX) $(FLAG) -o merge mainA51.cpp -lntl
