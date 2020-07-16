#!/bin/sh


###### USAGE #######

# Create (and push) ANNOTATED tags in git prefixed with "v" (eg "v1.2.0") to define the 'marketing' version number for commits going forward
# The following variables will become available from your Info.plist (and by extension the project's general settings)

# -- SHORT_VERSION : whatever the tag was, eg 1.2.0 - without the "v"
# -- LONG_VERSION : is in the form 1.2.0.xx (where xx is the number of commits since the tag)
# -- REV_COUNT : Monotonously increasing build numbers which play nice with Apple Testflight
# -- GIT_HASH : the short-form hash of the current commit
# -- BUMPS : the number of commits since the last tag
# -- DIRTY : 'true' if there are uncommitted changes, otherwise 'false'

# in DEVELOPMENT builds
# -dirty will be appended to the variables if there are uncommitted changes

# in RELEASE builds
# the script will FAIL if there are uncommitted changes

###### INSTALLATION #######

# Set the following in the Build Settings of the target:
# Info.plist Preprocessor Prefix File => $PROJECT_TEMP_DIR/revision.prefix
# Preprocess Info.plist file => Yes
# Optionally pass the -traditional flag to the Info.plist Other Preprocessor Flags build setting to stop URLs being turned into comments

# Create an aggregate target (called Autoversioning or whatever)
# Add a Run Script phase
# Add the line "./autoversioning.sh"
# Back in your main target, add the aggregate target to target dependencies
# ... this is because the script needs to run before the plist gets preprocessed

# In the General settings of the main target assign some variables how you want them:
# Version => SHORT_VERSION
# Build => REV_COUNT


echo "**** VERSIONING ****"

cd "$PROJECT_DIR"

described=`git describe --tags --dirty --match "[vV][0-9]*" --long`
described=`echo $described | sed 's/v//'`
taggedVersion=`echo $described | awk '{split($0,a,"-"); print a[1]}'`
gitHash=`echo $described | awk '{split($0,a,"-"); print a[3]}'`
bumps=`echo $described | awk '{split($0,a,"-"); print a[2]}'`
dirty=`echo $described | awk '{split($0,a,"-"); print a[4]}'`
revCount=`git rev-list HEAD --count`

# Make sure we have a version number from git - bail if we don't
if [[ -z "$taggedVersion" ]]; then
    echo "No version number from git"
    exit 1
fi

# If there are no commits since the last tag then bumps should be zero
if [[ -z "$bumps" ]]; then
    bumps="0"
    echo "No commits since last tag"
fi

# Say if it's a dirty build!
# Do not allow build to continue if we are in release configuration.
if [[ -n "$dirty" ]]; then
    echo "DIRTY BUILD!"
    if [[ "$CONFIGURATION" != "Debug" ]]; then
        exit 1
    fi
fi


shortVersion="$taggedVersion"
longVersion="$taggedVersion.$bumps"

if [[ "$CONFIGURATION" == "Debug" ]]; then
  if [[ -n "$dirty" ]]; then
    longVersion="$taggedVersion.$bumps-$dirty"
  fi
fi

echo "Git hash: $gitHash"
echo "Long version: $longVersion"
echo "Short version: $shortVersion"
echo "Bumps since last tag: $bumps"
echo "Revision count: $revCount"

cd "$PROJECT_TEMP_DIR"

echo "#define GIT_HASH $gitHash" > revision.prefix
echo "#define BUMPS $bumps" >> revision.prefix
echo "#define LONG_VERSION $longVersion" >> revision.prefix
echo "#define SHORT_VERSION $shortVersion" >> revision.prefix
echo "#define REV_COUNT $revCount" >> revision.prefix

cd "$PROJECT_DIR/$PROJECT_NAME"
touch "Info.plist"

exit 0
