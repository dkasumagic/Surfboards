const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const webui_de = b.dependency("webui", .{
        .target = target,
        .optimize = optimize,
    });

    const webui = webui_de.artifact("webui");

    const exe = b.addExecutable(.{
        .name = "Surfboards",
        .target = target,
        .optimize = optimize,
    });

    exe.addCSourceFiles(.{
        .files = &.{
            "src/frontend/app.cpp",
            "src/backend/matrixhandler.cpp",
            "src/backend/SVD.cpp",
            "src/backend/verilog_writer.cpp",
            "src/backend/synthesis_engine.cpp",
            // add more files here
        },
        .flags = &.{
            "-std=c++17",
            "-I/usr/include/eigen3",
        }
    });

    exe.addIncludePath(b.path("vendor/webui/include/"));


    exe.linkLibCpp();
    exe.linkLibrary(webui);

    b.installArtifact(exe);

    const run_cmd = b.addRunArtifact(exe);

    run_cmd.step.dependOn(b.getInstallStep());

    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);

}