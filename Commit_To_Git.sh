if [ "$#" -ne 1 ]; then
	echo "Error: Enter commit description."
	exit -1
fi


git add "."
git commit -m "$1"
git push -u origin master