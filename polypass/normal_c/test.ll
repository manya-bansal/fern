; ModuleID = 'test.c'
source_filename = "test.c"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "arm64-apple-macosx14.0.0"

; Function Attrs: noinline nounwind optnone ssp uwtable(sync)
define i32 @CNAME(i64 noundef %n, i64 noundef %dummy0, i64 noundef %dummy1, float noundef %da, ptr noundef %x, i64 noundef %inc_x, ptr noundef %y, i64 noundef %inc_y, ptr noundef %dummy, i64 noundef %dummy2) #0 {
entry:
  %retval = alloca i32, align 4
  %n.addr = alloca i64, align 8
  %dummy0.addr = alloca i64, align 8
  %dummy1.addr = alloca i64, align 8
  %da.addr = alloca float, align 4
  %x.addr = alloca ptr, align 8
  %inc_x.addr = alloca i64, align 8
  %y.addr = alloca ptr, align 8
  %inc_y.addr = alloca i64, align 8
  %dummy.addr = alloca ptr, align 8
  %dummy2.addr = alloca i64, align 8
  %i = alloca i64, align 8
  %ix = alloca i64, align 8
  %iy = alloca i64, align 8
  store i64 %n, ptr %n.addr, align 8
  store i64 %dummy0, ptr %dummy0.addr, align 8
  store i64 %dummy1, ptr %dummy1.addr, align 8
  store float %da, ptr %da.addr, align 4
  store ptr %x, ptr %x.addr, align 8
  store i64 %inc_x, ptr %inc_x.addr, align 8
  store ptr %y, ptr %y.addr, align 8
  store i64 %inc_y, ptr %inc_y.addr, align 8
  store ptr %dummy, ptr %dummy.addr, align 8
  store i64 %dummy2, ptr %dummy2.addr, align 8
  store i64 0, ptr %i, align 8
  %0 = load i64, ptr %n.addr, align 8
  %cmp = icmp slt i64 %0, 0
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  store i32 0, ptr %retval, align 4
  br label %return

if.end:                                           ; preds = %entry
  %1 = load float, ptr %da.addr, align 4
  %conv = fpext float %1 to double
  %cmp1 = fcmp oeq double %conv, 0.000000e+00
  br i1 %cmp1, label %if.then3, label %if.end4

if.then3:                                         ; preds = %if.end
  store i32 0, ptr %retval, align 4
  br label %return

if.end4:                                          ; preds = %if.end
  store i64 0, ptr %ix, align 8
  store i64 0, ptr %iy, align 8
  store i32 0, ptr %retval, align 4
  br label %return

return:                                           ; preds = %if.end4, %if.then3, %if.then
  %2 = load i32, ptr %retval, align 4
  ret i32 %2
}

attributes #0 = { noinline nounwind optnone ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+ccdp,+ccidx,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8a,+zcm,+zcz" }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"uwtable", i32 1}
!3 = !{i32 7, !"frame-pointer", i32 1}
!4 = !{!"clang version 20.0.0git (https://github.com/llvm/llvm-project.git 0be1883c36fc19e4020fea12902481c3dd3436d2)"}
