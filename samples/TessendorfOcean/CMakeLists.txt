set (SRC
	src/Main.cpp
	src/Ocean.cpp
)

set (HEADERS
	include/Ocean.h
)

sketch3d_add_sample(Sample_TessendorfOcean
					SOURCES ${SRC}
					HEADERS ${HEADERS}
)