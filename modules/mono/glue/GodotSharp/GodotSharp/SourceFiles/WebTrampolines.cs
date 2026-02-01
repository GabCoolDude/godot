using System;
using System.Runtime.InteropServices;

namespace GodotPlugins.Game
{
    internal static partial class Initializer
    {
        // Generate web trampolines, in alphabetical order
        // C# doesn't automatically generate them if delegate is of type delegate* unmanaged<[contains long or ulong]>
        // I don't really see any being added any time soon so manually declaring them should be fine
        [UnmanagedFunctionPointer(CallingConvention.Winapi)]
        private delegate IntPtr classdb_get_method_bind_sig(IntPtr _1, IntPtr _2, long _3);
        [UnmanagedFunctionPointer(CallingConvention.Winapi)]
        private delegate Godot.NativeInterop.godot_bool InitializeFromGameProject_sig(IntPtr _0, IntPtr _1, IntPtr _2, int _3);

        [UnmanagedFunctionPointer(CallingConvention.Winapi)]
        public delegate IntPtr godotsharp_method_bind_get_method_with_compatibility_sig(IntPtr _0, IntPtr _1, ulong _2);
        [UnmanagedFunctionPointer(CallingConvention.Winapi)]
        public delegate long godotsharp_variant_as_int_sig(IntPtr _0);
        [UnmanagedFunctionPointer(CallingConvention.Winapi)]
        public delegate void godotsharp_packed_byte_array_decompress_sig(IntPtr _0, long _1, int _2, IntPtr _3);
        [UnmanagedFunctionPointer(CallingConvention.Winapi)]
        public delegate void godotsharp_packed_byte_array_decompress_dynamic_sig(IntPtr _0, long _1, int _2, IntPtr _3);
        [UnmanagedFunctionPointer(CallingConvention.Winapi)]
        internal delegate IntPtr godotsharp_instance_from_id_sig(ulong _0);
        [UnmanagedFunctionPointer(CallingConvention.Winapi)]
        internal delegate uint godotsharp_rand_from_seed_sig(ulong _0, IntPtr _1);
        [UnmanagedFunctionPointer(CallingConvention.Winapi)]
        public delegate void godotsharp_seed_sig(ulong _0);
        [UnmanagedFunctionPointer(CallingConvention.Winapi)]
        public delegate long godotsharp_string_size_sig(IntPtr _0);
        [UnmanagedFunctionPointer(CallingConvention.Winapi)]
        public delegate long godotsharp_packed_byte_array_size_sig(IntPtr _0);
        [UnmanagedFunctionPointer(CallingConvention.Winapi)]
        public delegate long godotsharp_packed_int32_array_size_sig(IntPtr _0);
        [UnmanagedFunctionPointer(CallingConvention.Winapi)]
        public delegate long godotsharp_packed_int64_array_size_sig(IntPtr _0);
        [UnmanagedFunctionPointer(CallingConvention.Winapi)]
        public delegate long godotsharp_packed_float32_array_size_sig(IntPtr _0);
        [UnmanagedFunctionPointer(CallingConvention.Winapi)]
        public delegate long godotsharp_packed_float64_array_size_sig(IntPtr _0);
        [UnmanagedFunctionPointer(CallingConvention.Winapi)]
        public delegate long godotsharp_packed_string_array_size_sig(IntPtr _0);
        [UnmanagedFunctionPointer(CallingConvention.Winapi)]
        public delegate long godotsharp_packed_vector2_array_size_sig(IntPtr _0);
        [UnmanagedFunctionPointer(CallingConvention.Winapi)]
        public delegate long godotsharp_packed_vector3_array_size_sig(IntPtr _0);
        [UnmanagedFunctionPointer(CallingConvention.Winapi)]
        public delegate long godotsharp_packed_vector4_array_size_sig(IntPtr _0);
        [UnmanagedFunctionPointer(CallingConvention.Winapi)]
        public delegate long godotsharp_packed_color_array_size_sig(IntPtr _0);
        [UnmanagedFunctionPointer(CallingConvention.Winapi)]
        public delegate long godotsharp_array_size_sig(IntPtr _0);
    }
}
