package fr.acth2.a2wm.utils.manager;

public class SecurityManager extends java.lang.SecurityManager {
    public static int exitCode = 0;

    @Override
    public void checkExit(int status) {
        exitCode = status;

        super.checkExit(status);
    }

    @Override
    public void checkPermission(java.security.Permission perm) {
        if ("shutdownHooks".equals(perm.getName()) ||
                "accessDeclaredMembers".equals(perm.getName()) ||
                "suppressAccessChecks".equals(perm.getName()) ||
                "exitVM".equals(perm.getName()) ||
                "java.util.logging.manager".equals(perm.getName()) ||
                "setContextClassLoader".equals(perm.getName()) ||
                "control".equals(perm.getName()) ||
                ("java.util.logging.LoggingPermission".equals(perm.getClass().getName())) ||
                ("java.util.PropertyPermission".equals(perm.getClass().getName()) &&
                        ("sun.util.logging.disableCallerCheck".equals(perm.getName()) ||
                                "user.home".equals(perm.getName())) &&
                        "read".equals(perm.getActions())
                ) ||
                ("java.io.FilePermission".equals(perm.getClass().getName()) &&
                        (perm.getActions().contains("read") || perm.getActions().contains("write"))
                ) ||
                (perm instanceof RuntimePermission && perm.getName().startsWith("getenv"))
        ) {
            return;
        }
        super.checkPermission(perm);
    }




}