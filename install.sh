#!/bin/bash
# dear packagers, there are 2 variables you can set whilst running this script
# INSTALL sets the path to copy and install the files to
# PREFIX sets the path in which this install will operate
#
# For example, an AUR ( http://aur.archlinux.org ) script would invoke it as follows
# INSTALL="$pkgdir" PREFIX="/usr" ./install.sh

if [ "${INSTALL}" == "" ]
then
	INSTALL=""
fi

if [ "${PREFIX}" == "" ]
then
	echo "You are installing sandbox globally, if you wish to abort, hit Ctrl-C now"
	echo ""
	echo "Enter installation prefix..."
	echo "default: /usr/local"
	read PREFIX

	if [ "${PREFIX}" == "" ]
	then
		PREFIX="/usr/local"
	fi

	echo "Sandbox will be installed in \"${PREFIX}/share/sandbox\""
	echo "Files will be copied to \"${INSTALL}${PREFIX}/share/sandbox\""
	echo "Press enter to continue"
	read CC
fi

if [ -e "${INSTALL}${PREFIX}/share/sandbox" ]
then
	echo "NOTE, \"${INSTALL}${PREFIX}/share/sandbox\" already exists - removing"
	rm -rf "${INSTALL}${PREFIX}/share/sandbox"
fi

echo "Creating directory \"${INSTALL}${PREFIX}/share/sandbox\""
mkdir -p "${INSTALL}${PREFIX}/share/sandbox"
if [ $? -ne 0 ]
then
	echo "Failed to create directory, do you have permission?"
	exit 1
fi

echo "Copying files to \"${INSTALL}${PREFIX}/share/sandbox\" (this may take a while)"

if [ -e .svn ]
then
	echo ".svn folder exists, using svn export..."
	svn export --quiet . "${INSTALL}${PREFIX}/share/sandbox"
else
	echo "No .svn folder available, copying current directory to destination..."
	cp -r . "${INSTALL}${PREFIX}/share/sandbox"
fi

if [ $? -ne 0 ]
then
	echo "Failed to copy files, do you have permission?"
	exit 1
fi

#just in case
mkdir -p "${INSTALL}${PREFIX}/bin"
echo "Creating symlink from \"${INSTALL}${PREFIX}/bin/sandbox\" to \"${PREFIX}/share/sandbox/sandbox_unix\""
if [ -e "${INSTALL}${PREFIX}/bin/sandbox" ]
then
	echo "NOTE, \"${INSTALL}${PREFIX}/bin/sandbox\" exists - removing"
	rm -f "${INSTALL}${PREFIX}/bin/sandbox"
fi

# we want to reference it via the prefix path
ln -s "${PREFIX}/share/sandbox/sandbox_unix" "${INSTALL}${PREFIX}/bin/sandbox"
if [ $? -ne 0 ]
then
	echo "Failed to create symlink, do you have permission?"
	exit 1
fi

echo "Setting file permissions to 0644 and owner to root..."
chmod -R a+X "${INSTALL}${PREFIX}/share/sandbox"
chmod -R og-w "${INSTALL}${PREFIX}/share/sandbox"

if [ $UID -eq 0 ]
then
	chown -R root:root "${INSTALL}${PREFIX}/share/sandbox"
else
	echo "Warning, cannot set owner of \"${INSTALL}${PREFIX}/share/sandbox\" to root!"
fi

# this is to undo the above partially so everyone can access these directories


echo "Setting executable bits on scripts and binaries (just in case)"
chmod +x ${INSTALL}${PREFIX}/share/sandbox/sandbox_unix
chmod +x ${INSTALL}${PREFIX}/share/sandbox/bin/sandbox_{client,server}_{32,64}_*

mkdir -p "${INSTALL}${PREFIX}/share/pixmaps"
echo "Installing pixmaps to ${INSTALL}${PREFIX}/share/pixmaps"
cp ./linux/*.png "${INSTALL}${PREFIX}/share/pixmaps"

mkdir "${INSTALL}${PREFIX}/share/applications"
echo "Installing .desktop files to ${INSTALL}${PREFIX}/share/applications"
cp ./linux/*.desktop "${INSTALL}${PREFIX}/share/applications"

echo "Sandbox has been successfully installed"
echo ""
echo "To fully uninstall sandbox, run the following commands"
echo "rm \"${INSTALL}${PREFIX}/bin/sandbox\""
echo "rm -r \"${INSTALL}${PREFIX}/share/sandbox\""
echo "rm ${INSTALL}${PREFIX}/share/pixmaps/sandbox_*"
echo "rm ${INSTALL}${PREFIX}/share/applications/sandbox_*"
echo ""
echo "To run sandbox, simply enter \"sandbox\" into your shell of choice"
echo "or select one of the launcher from your desktop's menu under the \"Games\" subtree"
