Import("env")

# do this only to wired target
TARGET_ENV = "esp32cam"

if env["PIOENV"] == TARGET_ENV:

    def initial_erase_and_flash(source, target, env):
        env.Execute("platformio run -e %s -t erase" % TARGET_ENV)
        env.Execute("platformio run -e %s -t upload" % TARGET_ENV)
        env.Execute("platformio run -e %s -t uploadfs" % TARGET_ENV)
        env.Execute("platformio device monitor -e %s" % TARGET_ENV)

    env.AddCustomTarget(
        name="initial_erase_and_flash",
        dependencies=None,
        actions=[initial_erase_and_flash],
        title="Initial Erase + Flash + FS + Monitor",
        description="Erase, upload firmware, upload filesystem, then open monitor"
    )