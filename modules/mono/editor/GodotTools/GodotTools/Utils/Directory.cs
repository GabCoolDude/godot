using System.IO;
using Godot;

namespace GodotTools.Utils
{
    public static class Directory
    {
        private static string GlobalizePath(this string path)
        {
            return ProjectSettings.GlobalizePath(path);
        }

        public static bool Exists(string path)
        {
            return System.IO.Directory.Exists(path.GlobalizePath());
        }

        /// Create directory recursively
        public static DirectoryInfo CreateDirectory(string path)
        {
            return System.IO.Directory.CreateDirectory(path.GlobalizePath());
        }

        public static void Delete(string path, bool recursive)
        {
            System.IO.Directory.Delete(path.GlobalizePath(), recursive);
        }

        public static void CopyDirectory(string sourceDir, string destinationDir, bool recursive)
        {
            var globalDestDir = destinationDir.GlobalizePath();

            // Get information about the source directory
            var dir = new DirectoryInfo(sourceDir.GlobalizePath());

            // Check if the source directory exists
            if (!dir.Exists)
                throw new DirectoryNotFoundException($"Source directory not found: {dir.FullName}");

            // Cache directories before we start copying
            DirectoryInfo[] dirs = dir.GetDirectories();

            // Create the destination directory
            System.IO.Directory.CreateDirectory(globalDestDir);

            // Get the files in the source directory and copy to the destination directory
            foreach (FileInfo file in dir.GetFiles())
            {
                string targetFilePath = Path.Combine(globalDestDir, file.Name);
                GodotTools.Utils.File.Copy(file.FullName, targetFilePath);
            }

            // If recursive and copying subdirectories, recursively call this method
            if (recursive)
            {
                foreach (DirectoryInfo subDir in dirs)
                {
                    string newDestinationDir = Path.Combine(globalDestDir, subDir.Name);
                    CopyDirectory(subDir.FullName, newDestinationDir, true);
                }
            }
        }

        public static string[] GetDirectories(string path, string searchPattern, SearchOption searchOption)
        {
            return System.IO.Directory.GetDirectories(path.GlobalizePath(), searchPattern, searchOption);
        }

        public static string[] GetFiles(string path, string searchPattern, SearchOption searchOption)
        {
            return System.IO.Directory.GetFiles(path.GlobalizePath(), searchPattern, searchOption);
        }
    }
}
