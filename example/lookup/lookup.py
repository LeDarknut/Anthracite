def lookup(function, name, compBit, unitDivBit, raisingBit, byteNum, signed) :
	
	unitDiv = 1 / (2 ** unitDivBit)
	raising = (2 ** raisingBit)

	with open(name + ".lkt", "wb") as f :
		
		for n in range(0, (2 ** compBit) + 1) :
			
			v = function(n * unitDiv)
			
			v = round(v * raising)
			
			print(v)
			
			v= v.to_bytes(byteNum, byteorder = "little", signed = signed)
			
			f.write(v)
		
def byte_lookup(function, name, compBit, unitDivBit, raisingBit) :
	
	lookup(function, name, compBit, unitDivBit, raisingBit, 1, False)
	
def sbyte_lookup(function, name, compBit, unitDivBit, raisingBit) :
	
	lookup(function, name, compBit, unitDivBit, raisingBit, 1, True)
	
def word_lookup(function, name, compBit, unitDivBit, raisingBit) :
	
	lookup(function, name, compBit, unitDivBit, raisingBit, 2, False)
	
def sword_lookup(function, name, compBit, unitDivBit, raisingBit) :
	
	lookup(function, name, compBit, unitDivBit, raisingBit, 2, True)
	
def dword_lookup(function, name, compBit, unitDivBit, raisingBit) :
	
	lookup(function, name, compBit, unitDivBit, raisingBit, 4, False)
	
def sdword_lookup(function, name, compBit, unitDivBit, raisingBit) :
	
	lookup(function, name, compBit, unitDivBit, raisingBit, 4, True)
	
def qword_lookup(function, name, compBit, unitDivBit, raisingBit) :
	
	lookup(function, name, compBit, unitDivBit, raisingBit, 8, False)
	
def sqword_lookup(function, name, compBit, unitDivBit, raisingBit) :
	
	lookup(function, name, compBit, unitDivBit, raisingBit, 8, True)

import math

def func(n) :
	
	return math.sin(n * 2 * math.pi)
	
sword_lookup(func, "sin", 8, 8, 14)