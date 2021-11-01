Import(Split("env PREFIX DOCDIR CLIENTS EXAMPLESDIR SERVER"))

env.Install(dir=DOCDIR,source=Split("""BETA.README COPYRIGHT ChangeLog
FILES INFO INSTALL MACHINES TODO
"""))

if SERVER:
  env.Install(dir=EXAMPLESDIR,source="fspd.conf")

# *************** Targets ****************

#Add install target
env.Alias("install", PREFIX+'/bin')

#Add build target
env.Alias("build", Split('server/ clients/ tests/ doc/ man/') )

#Change default target to build
env.Default(None)
env.Default("build")

Export("EXAMPLESDIR")
