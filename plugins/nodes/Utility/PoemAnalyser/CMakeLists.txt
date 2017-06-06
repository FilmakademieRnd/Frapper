project(PoemAnalyser)

# PoemAnalyser
set( res_header
			PoemAnalyserNode.h
			PoemAnalyserNodePlugin.h
			)

set( res_moc
			PoemAnalyserNode.h
			PoemAnalyserNodePlugin.h
			)

set( res_source
			PoemAnalyserNode.cpp
			PoemAnalyserNodePlugin.cpp
			)

set( res_description
			poemanalyser.xml
			)

include( add_project )