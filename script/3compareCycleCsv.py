import sys

total_keys = set()
alldicts = []
for inputf in sys.argv[1:]:
	inf = open(inputf, 'r')

	lines = inf.readlines()
	dictt = {}

	for line in lines:
		if 'Frame' in line or 'Others' in line or 'Time' in line or len(line) < 3: continue
		[name, time, kcalls] = line.split('\t')
		time_per_call = float(time)/float(kcalls)/1000
		try:
			dictt[name].append([float(time),float(kcalls)*1000,time_per_call])
		except:
			dictt[name] = [[float(time),float(kcalls)*1000,time_per_call]]
		total_keys = total_keys | set([name])

	alldicts.append(dictt)

diff = []
fout = open('comparison.csv','w')
for key in sorted(total_keys):
	try:
		avgs = []
		times = []
		calls = []
		for dictt in alldicts:
			summ_tpc = 0.0
			summ_t = 0.0
			summ_c = 0.0
			count = 0
			for frame in dictt[key]:
				summ_t += frame[0]
				summ_c += frame[1]
				summ_tpc += frame[2]

				count += 1
			avgs.append(summ_tpc/count)
			times.append(summ_t/count)
			calls.append(summ_c/count)
		print >> fout, '\t'.join([str(x) for x in [key] + times + calls + avgs ])
	except:
		diff.append(key)

print >> fout, '\n\n\n\n'
for d in diff:
	print >> fout, d
