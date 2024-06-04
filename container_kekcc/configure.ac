AC_INIT(unpacker_container.def.in)

GIT_USERNAME=`git config --get user.name`
GIT_USEREMAIL=`git config --get user.email`

if test [ "X$GIT_USERNAME" != "X" ] ; then
    echo "found user.name: $GIT_USERNAME"
    DEFINE_GIT_USERNAME="$GIT_USERNAME"
else
    exit
fi

if test "X$GIT_USEREMAIL" != "X"  ; then
    echo "found user.name: $GIT_USEREMAIL"
    DEFINE_GIT_USEREMAIL="$GIT_USEREMAIL"
else
    exit
fi

if test "X$GIT_SECRET_TOKEN" != "X"  ; then
    echo "GIT_SECRET_TOKEN is defined: ****"
    DEFINE_GIT_CLONE="git clone https://oauth2:${GIT_SECRET_TOKEN}@git.t2k.org/sfgd-daq/unpacking.git"
else
    echo "GIT_SECRET_TOKEN is not defined"
    echo "!!! You will be asked to fill your username and password !!!"
    DEFINE_GIT_CLONE="git clone https://git.t2k.org/sfgd-daq/unpacking.git"
fi

AC_SUBST(DEFINE_GIT_USERNAME)
AC_SUBST(DEFINE_GIT_USEREMAIL)
AC_SUBST(DEFINE_GIT_CLONE)

AC_CONFIG_FILES([unpacker_container.def])
AC_OUTPUT
