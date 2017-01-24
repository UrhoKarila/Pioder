#!/usr/bin/python

import sys, os, struct
#  Can use struct.pack(">B", val) to convert val into an appropriate byte representation
#  Can use struct.pack(">H", val) to convert val into an appropriate 2-byte representation

def menufromids(prompt, ids, options):
	clear()
	print prompt
	for i in range(len(ids)):
		opt = str(rpadstring(ids[i],3)) + ': ' + str(options[i])
		print opt
	print '\n---------\n'
	try:
		choice =  input('  ?: ')
		assert type(choice) is int
		choice = int(choice)
		assert int(choice) in ids
	except:
#		pause()
#		print "Your choice was " + str(choice)
#		print 'type(choice) is ' + str(type(choice))
#		print "Choice in Ids: " + str(int(choice) in ids)
#		pause()
		print "Please enter a valid choice\n"
		pause()
		choice = menu(prompt, options) + 1

	return choice

def menu(prompt, options):
	ids = range(1, len(options)+1)
	return menufromids(prompt, ids, options) - 1

def pause():
	raw_input('--------- \n Press Enter to continue')

def byte(i):
	return struct.pack(">B", i)

def bigbyte(i):
	return struct.pack(">H", i)

def boolbit(i):
	if i:
		return 0x01
	else:
		return 0x00

def clamp(i):
	if i > 255:
		return 255
	if i < 0:
		return 0
	return i

def bigclamp(i):
	if i > 65535:
		return 65535
	if i < 0:
		return 0
	return i

def padstring(text, targetlen):
	text = str(text)
	while len(text) < targetlen:
		text += ' '
	return text

def rpadstring(text, targetlen):
	text = str(text)
	while len(text) < targetlen:
		text = ' ' + text
	return text

# //take Key/Value pairs of config options to present in a menu
def submenu(options):
	for i in options.keys():
		print(padstring(i, 4) + ': ' + str(options[i]))
	selection = int(raw_input('Select an option: '))
	return selection

def clear():
	os.system('clear')

def addeffect():
	clear()
#	effect_list = getalleffects()
	already_exists = ("Lighting")	#Printable list of influences & effects
	#influences = []						#Keep track of used influences for client-side checks
#	for effect in effect_list:
	# 	already_exists += ('\n' + padstring(effect[2], 5) + str(effect[1]))
	# 	influences.append(effect[2])
	# already_exists += '\n------------\n'
	
#	for line in already_exists:
#		print line

	fields = ['port', 'red', 'green', 'blue', 'transition type', 'transition duration', 'is alert']
	print fields
	options = list(fields) #create a new list. Otherwise changing options changes fields
	# options.append('add essence requirements')
	# print options
	# options.append('add ingredient requirements')
	options.append('Send')


	values = {}
	for field in fields:
		values[field] = 0
	# essence_requirements = []
	# ingredient_requirements = []

	while True:
		prompt = already_exists
		for value in values:
			prompt += '\n' + rpadstring(str(value) + ':', 20) + repr(values[value])
		prompt += '\n Choose a value to input'

		choice = menu(prompt, options)
		if   options[choice] == 'port':
			values['port'] 	= submenu({'0': "all",
									 '1': "left",
									 '2': "right",
									 '255': "global color balance"})
		elif   options[choice] == 'is alert':
			values['port'] 	= submenu({'0': False,
									 '1': True})
		# elif options[choice] == 'influence':
		# 	inf			= int(raw_input('Enter a unique influence for the effect (int): '))
		# 	if (inf in influences):
		# 		print 'Influence value already exists in db'
		# 		pause()
		# 	else:
		# 		values['influence'] = int(inf)
		elif options[choice] == 'red':
			values['red'] 	= clamp(int(raw_input('Give a value: ')))
		elif options[choice] == 'green':
			values['green']  	= clamp(int(raw_input('Give a value: ')))
		elif options[choice] == 'blue':
			values['blue']   	= clamp(int(raw_input('Give a value: ')))
		elif options[choice] == 'transition duration':
			values['transition duration']	= bigclamp(int(raw_input('Give a value: ')))
		elif options[choice] == 'transition type':
			values['transition type'] = submenu({'0': "linear",
												 '1': "QuadIn",
												 '2': "QuadOut",
												 '3': "QuadInOut"})

		# elif options[choice] == 'add ingredient requirements':
		# 	ing = getallingredients()
		# 	for i in ing:
		# 		print(padstring(i[0], 2), i[1])
		# 	while True:
		# 		ingredient = raw_input('Select an ingredient ID: ')
		# 		if ingredient == '':
		# 			break
		# 		ingredient_requirements.append(ingredient)
		
		elif options[choice] == 'Send':
			# good_to_go = True
			# for value in values:
			# 	print "Checking " + repr(value) + ", is " + str(values[value])
			# 	if values[value] == None:
			# 		good_to_go = False
			# 		print 'please enter a value for ' + str(value)
			# 		pause
			# if good_to_go:
			return values

values = addeffect()

print ("Should send the following message: " + 
	repr(byte(values['port'])) + 
	repr(boolbit(values['is alert'])) + 
	repr(bigbyte(values['transition duration'])) + 
	repr(byte(values['transition type'])) + 
	repr(byte(values['red'])) + 
	repr(byte(values['green'])) + 
	repr(byte(values['blue']))
	)