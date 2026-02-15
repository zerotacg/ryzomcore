cgc -entry fxaa_pp fxaa_pp.cg -profile arbfp1 -O3 -fastmath -fastprecision -o fxaa_pp_arbfp1.txt
cgc -entry fxaa_pp fxaa_pp.cg -profile ps_2_x -O3 -fastmath -fastprecision -o fxaa_pp_ps_2_0.txt
cgc -entry fxaa_vp fxaa_vp.cg -profile arbvp1 -fastmath -fastprecision -o fxaa_vp_arbvp1.txt
cgc -entry stereo_debug_pp stereo_debug_pp.cg -profile arbfp1 -O3 -fastmath -fastprecision -o stereo_debug_pp_arbfp1.txt
cgc -entry stereo_debug_pp stereo_debug_pp.cg -profile ps_2_0 -O3 -fastmath -fastprecision -o stereo_debug_pp_ps_2_0.txt