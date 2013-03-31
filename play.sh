if [ -f "code/CamelRace" ]
then
	cd config; ../code/CamelRace;
else
	echo "Project not built, first run:"
	echo "$ sh ./autogen.sh"
	echo "$ make"
fi
