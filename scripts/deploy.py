from SCons.Script import Import
Import("env")
import os

# Pi target (change host/user if needed)
PI_USER = "pi"
PI_HOST = "raspberrypi.local"
DEPLOY_DIR = "~/deploy/bin"

def after_build(source, target, env):
    prog_path = target[0].get_abspath()
    print(f"Deploying {prog_path} to {PI_USER}@{PI_HOST}:{DEPLOY_DIR}")
    os.system(f"scp {prog_path} {PI_USER}@{PI_HOST}:{DEPLOY_DIR}/program")
    os.system(
        "ssh {user}@{host} 'pkill program || true; chmod +x {dir}/program; "
        "nohup {dir}/program > ~/deploy/last.log 2>&1 &'"
        .format(user=PI_USER, host=PI_HOST, dir=DEPLOY_DIR)
    )

env.AddPostAction("$BUILD_DIR/${PROGNAME}", after_build)Import("env")
import os

# Pi target
PI_USER = "pi"
PI_HOST = "raspberrypi.local"
DEPLOY_DIR = "~/deploy/bin"

def after_build(source, target, env):
    prog_path = target[0].get_abspath()
    print(f"Deploying {prog_path} to {PI_USER}@{PI_HOST}:{DEPLOY_DIR}")
    os.system(f"scp {prog_path} {PI_USER}@{PI_HOST}:{DEPLOY_DIR}/program")
    os.system(f"ssh {PI_USER}@{PI_HOST} 'pkill program || true; chmod +x {DEPLOY_DIR}/program; nohup {DEPLOY_DIR}/program > ~/deploy/last.log 2>&1 &'")

env.AddPostAction("$BUILD_DIR/${PROGNAME}", after_build)
