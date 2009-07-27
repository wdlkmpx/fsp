Import(Split("env PREFIX"))

# *************** Targets ****************

#Add install target
env.Alias("install",[ PREFIX+'/bin', PREFIX+'/man'] )

#Add build target
env.Alias("build", Split('server/fspd clients/ contrib/ tests/') )

#Change default target to build
env.Default(None)
env.Default("build")
