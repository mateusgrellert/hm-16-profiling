import Configuration
import os
from joblib import Parallel, delayed  
import math

def runParallelSims(sequence,numFrames, gopStructure, qp, pathToBin, optParams):
	[gopPath, seqPath, resultsPath] = treatConfig(sequence, gopStructure, qp)
	if os.path.isfile(resultsPath) or os.path.isfile(resultsPath.lower()) :
		return
	if numFrames:
		optParams += ' -f ' + str(numFrames)

	cmdLine = '%s -c %s -c %s -q %s %s > hm_out.txt  2>&1 ' % (pathToBin, gopPath, seqPath, qp, optParams)	
	#cmdLine = '%s -c %s -c %s -q %s %s > %s ' % (pathToBin, gopPath, seqPath, qp, optParams, resultsPath)
	#print cmdLine
	os.system(cmdLine)
	
def treatConfig(sequence, gopStructure, qp):
	gopStructure = gopStructure.split('.cfg')[0]
	sequence = sequence.split('.cfg')[0]
	cfgPath = Configuration.cfgPath.rstrip('/')

	sequencePath = Configuration.sequencePath.rstrip('/')

	gopPath = cfgPath + '/' + gopStructure + '.cfg'
	seqPath = sequencePath + '/' + sequence + '.cfg'
	
	
	resultsPath = Configuration.pathToCsv + '_' + str(qp) + '.csv'
	
	return [gopPath, seqPath, resultsPath]

def parseOutput(resultsPath):
	hmResults = open(resultsPath, 'r')
	lines = hmResults.readlines()
	hmResults.close()
	stats_dict = {}
	count_dict = {}
	for l in lines:
		if 'Frame' in l or 'Time' in l or 'Others' in l or len(l )< 3: continue
#		print l
		[name,time,calls]= l.split('\t')
		
		[type,oper,dim] = name.split('_')
		[w,h] = dim.split('x')
		if 'sub' in h:
		#	h = int(h[:-3])/2
			h = int(h[:-3])
		w = int(w)
		h = int(h)
		if type == 'TR':
			depth = 5 - int(math.log(max(w,h),2))
		else:
			depth = 6 - int(math.log(max(w,h),2))
		depth = 3 if depth > 3 else depth
		if type not in stats_dict.keys():
			stats_dict [type] = {}
			count_dict [type] = {}
		if oper not in stats_dict[type].keys():
			stats_dict[type][oper] = [0,0,0,0]
			count_dict[type][oper] = [0,0,0,0]

		stats_dict[type][oper][depth] += float(time)/float(calls)/1000.0
		count_dict[type][oper][depth] += 1
		#stats_dict[type][oper][0][depth] = float(time)
		#stats_dict[type][oper][1][depth] = float(calls)
	return [stats_dict, count_dict]
	
for gopStructure in Configuration.gopStructureList:
	for [sequence, numFrames] in Configuration.sequenceList:

		cycleResults = []
		optParams = Configuration.optParamsRef
		pathToBin = Configuration.pathToRefBin
		if Configuration.RUN_PARALLEL:
			Parallel(n_jobs=Configuration.NUM_THREADS)(delayed(runParallelSims)(sequence,numFrames, gopStructure, qp, pathToBin, optParams) for qp in Configuration.qpList)
		else:
			for qp in Configuration.qpList:
				runParallelSims(sequence,numFrames, gopStructure, qp, pathToBin, optParams) 

		for qp in Configuration.qpList:
			[gopPath, seqPath, resultsPath] = treatConfig(sequence, gopStructure, qp)
			overall_stats = parseOutput(resultsPath)

			cycleResults.append(overall_stats)
			
type_avg = {}
oper_avg = {}
for [stats, count] in cycleResults:
	for type in stats.keys():
		oper_total = [0,0,0,0]
		count_oper_total = [0,0,0,0]
		for oper in stats[type].keys():
			if type not in type_avg.keys():
				type_avg [type] = [0,0,0,0]
			if type not in oper_avg.keys():
				oper_avg [type] = {}
			if oper not in oper_avg[type].keys():
				oper_avg[type][oper] = [0,0,0,0]
			for d in range(4):
				#print count[type][oper][d], stats[type][oper][d] 
				if  count[type][oper][d]:
					oper_avg[type][oper][d] = stats[type][oper][d] / count[type][oper][d]
				oper_total[d] += stats[type][oper][d]
				count_oper_total[d] += count[type][oper][d]
		for d in range(4):
			if count_oper_total[d]:
				type_avg[type][d] = oper_total[d] / count_oper_total[d]
				
for type in type_avg.keys():
	print type
	for d in range(len(type_avg[type])):
		print d, '\t', type_avg[type][d]*1000000.0
		
for type in oper_avg.keys():
	print type
	for oper in oper_avg[type].keys():
		print oper
		for d in range(len(oper_avg[type][oper])):
			print d, '\t', oper_avg[type][oper][d]*1000000.0
