Import(Split("env PREFIX VERSION PACKAGE TARBALL"))

# *************** Targets ****************

#Add install target
env.Alias("install",[ PREFIX+'/bin', PREFIX+'/man'] )

#Add build target
env.Alias("build", Split('server/fspd clients/ contrib/ tests/') )

#Change default target to build
env.Default(None)
env.Default("build")

#Add dist target
env.Replace(TARFLAGS = '-c -z')
env.Alias("dist",TARBALL)
#Clean tarball when doing build clean
env.Clean("build",TARBALL)
