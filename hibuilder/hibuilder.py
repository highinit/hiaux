import glob
import json

class HiBuilder:
	_CCFLAGS_SET = set()
	_LINKFLAGS_SET = set()
	_CPPPATH_SET = set()
	_CPPFILES_SET = set()
	_LIBS_NAMES_SET = set()

	_target_name = ""

	def __init__(self, _target_name):
		self._target_name = _target_name

	def connectLib(self, _hiconn_file_path):
		config_js = ''
		with open(_hiconn_file_path+'connect.hiconn') as f:
			config_js = f.read() #.replace('\n','')
		config = json.loads(config_js)
		
		if config["_NAME"] in self._LIBS_NAMES_SET:
			return

		print "Parsing " + _hiconn_file_path + "connect.hiconn"

		if '_CPPFILES' in config.keys():
			for cppfile in config['_CPPFILES']:
				if cppfile['type'] == "Glob":
					for file in glob.glob(_hiconn_file_path+cppfile['mask']):
						if not file in self._CPPFILES_SET:
							self._CPPFILES_SET.add(file)
				if cppfile['type'] == "file":
					if _hiconn_file_path+cppfile['file'] not in self._CPPFILES_SET:
						self._CPPFILES_SET.add(_hiconn_file_path+cppfile['file'])

		if '_CPPPATH_ABSOLUTE' in config.keys():
			for cpppath in config['_CPPPATH_ABSOLUTE']:
				if cpppath not in self._CPPPATH_SET:
					self._CPPPATH_SET.add(cpppath)

		if '_LINKFLAGS' in config.keys():
			for linkflag in config['_LINKFLAGS']:
				if linkflag not in self._LINKFLAGS_SET:
					self._LINKFLAGS_SET.add(linkflag)

		if '_CCFLAGS' in config.keys():
			for ccflag in config['_CCFLAGS']:
				if ccflag not in self._CCFLAGS_SET:
					self._CCFLAGS_SET.add(ccflag)
		
		if '_DEPS' in config.keys():
			for dep in config['_DEPS']:
				self.connectLib(_hiconn_file_path+dep)
		
		self._LIBS_NAMES_SET.add(config["_NAME"])

	def buildProgram(self, Program):

		_CXX = "clang++"
		_CCFLAGS = ""
		_LINKFLAGS = ""
		_CPPPATH = []
		_CPPFILES = []

		for ccflag in self._CCFLAGS_SET:
			_CCFLAGS += " " + ccflag
			print ccflag

		for linkflag in self._LINKFLAGS_SET:
			_LINKFLAGS += " " + linkflag
			print linkflag

		for cpppath in self._CPPPATH_SET:
			_CPPPATH.append(cpppath)
			print cpppath

		for cppfile in self._CPPFILES_SET:
			_CPPFILES.append(cppfile)

		Program(self._target_name, 
		_CPPFILES, CCFLAGS=_CCFLAGS, LINKFLAGS=_LINKFLAGS, CXX = _CXX,
		CPPPATH = _CPPPATH)

	def addLinkFlags(self, linkflags):
		for linkflag in linkflags:
			if linkflag not in self._LINKFLAGS_SET:
				self._LINKFLAGS_SET.add(linkflag)
	
	def addCCFlags(self, ccflags):
		for ccflag in ccflags:
			if ccflag not in self._CCFLAGS_SET:
				self._CCFLAGS_SET.add(ccflag)

	def addPath(self, path):
		if path not in self._CPPPATH_SET:
			self._CPPPATH_SET.add(path)

	def addCppFiles(self, cppfiles):
		for cppfile in cppfiles:
			if cppfile not in self._CPPFILES_SET:
				self._CPPFILES_SET.add(cppfile)

	def addCppGlob(self, mask):
		for cppfile in glob.glob(mask):
			if cppfile not in self._CPPFILES_SET:
				self._CPPFILES_SET.add(cppfile)
