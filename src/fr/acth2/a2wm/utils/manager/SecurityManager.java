package fr.acth2.a2wm.utils.manager;


/*
DONT ALLOW:
    Network operations (SocketPermission)
    Reflection operations (except the specific ones)
    Class loading operations (except setContextClassLoader)
    Native library loading (loadLibrary.*, load.*)
    Security policy modifications
    System clipboard access
    AWT event queue access
    Print job control
    Serialization permissions
    Majority of RuntimePermission
    Any other custom permissions from third-party libraries

DOES ALLOW:
    File operations
    Property reads/writes
    Environment variable access
    Specific RuntimePermission
    Logging permissions
    The specific shutdown/reflection/classloader permissions listed
    Thread operations (thread modification, thread group access)
 */

public class SecurityManager extends java.lang.SecurityManager {
    public static int exitCode = 0;

    @Override
    public void checkExit(int status) {
        exitCode = status;
        super.checkExit(status);
    }

    @Override
    public void checkPermission(java.security.Permission perm) {
        if (perm instanceof java.io.FilePermission ||
                perm instanceof java.util.PropertyPermission ||
                (perm instanceof RuntimePermission &&
                        (perm.getName().startsWith("getenv") ||
                                perm.getName().equals("getProtectionDomain") ||
                                perm.getName().equals("modifyThreadGroup") ||
                                perm.getName().equals("modifyThread") ||
                                perm.getName().equals("writeFileDescriptor") ||
                                perm.getName().equals("readFileDescriptor"))) ||
                "shutdownHooks".equals(perm.getName()) ||
                "accessDeclaredMembers".equals(perm.getName()) ||
                "suppressAccessChecks".equals(perm.getName()) ||
                "exitVM".equals(perm.getName()) ||
                "java.util.logging.manager".equals(perm.getName()) ||
                "setContextClassLoader".equals(perm.getName()) ||
                "control".equals(perm.getName()) ||
                "java.util.logging.LoggingPermission".equals(perm.getClass().getName())) {
            return;
        }
        super.checkPermission(perm);
    }
}