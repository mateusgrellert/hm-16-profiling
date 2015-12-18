import platform


def buildTestSet(path):
	inputFile = open(path, 'r')
	binList = []
	paramList = []
	testNameList = []
	for line in inputFile.readlines():
		if '#' in line or len(line.split()) < 3 : continue

		line = line.split()
		name = line[0]
		bin = line[1]
		params = ' --'.join(line[2:])
		binList.append(bin)
		testNameList.append(name)
		if params:
			paramList.append(' --'+params)
		else:
			paramList.append(' ')
	inputFile.close()
	
	return [testNameList, binList, paramList]


gopStructureList = ['encoder_lowdelay_main']
sequenceList = [['BQSquare', 64],['BlowingBubbles', 64], \
				['Traffic', 64],['PeopleOnStreet', 64], \
				['BasketballDrive', 64],['ParkScene', 64], \
				['RaceHorsesC', 64],['BQMall', 64], \
				['ChinaSpeed', 64],['BasketballDrillText', 64], \
				['Johnny', 64],  ['FourPeople',64]]
sequenceList = [['BQMall', 20]]
qpList = ['22', '27', '32', '37']
qpList = ['32']


cfgPath = '../cfg'
if platform.system() == 'Darwin':
	sequencePath = '/Users/grellert/hm-cfgs/cropped'
else:
	sequencePath = '/home/grellert/hm-cfgs/cropped'



pathToRefBin = '../bin/TAppEncoderStatic'
pathToCsv = '../bin/function_cycle_results'
optParamsRef = ''


RUN_PARALLEL = False
NUM_THREADS = 4
