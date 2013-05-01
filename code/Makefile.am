AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-I "./include" \
	$(OGF_CFLAGS) $(OgreBullet_CFLAGS) $(bullet_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = CamelRace

CamelRace_SOURCES = \
	src/main.cpp \
	src/GameScene.cpp \
	src/MainMenuScene.cpp \
	src/Model.cpp \
	src/SceneFactory.cpp \
	src/TeamMenuScene.cpp

CamelRace_LDADD = $(OGF_LIBS) $(OgreBullet_LIBS) $(bullet_LIBS)
