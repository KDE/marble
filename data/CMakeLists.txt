
if (QTONLY)
  if(WIN32)
    set(data_dir data)
  else(WIN32)
    set(data_dir ${CMAKE_INSTALL_PREFIX}/share/apps/marble/data)
  endif(WIN32)
else (QTONLY)
  set(data_dir ${DATA_INSTALL_DIR}/marble/data)
endif (QTONLY)


install (FILES
legend.html
legend.css
DESTINATION ${data_dir})

install (FILES
bitmaps/checkbox_checked.png
bitmaps/checkbox_empty.png
bitmaps/checkbox_disabled.png
bitmaps/city_1_orange.png
bitmaps/city_1_red.png
bitmaps/city_1_white.png
bitmaps/city_1_yellow.png
bitmaps/city_2_orange.png
bitmaps/city_2_red.png
bitmaps/city_2_white.png
bitmaps/city_2_yellow.png
bitmaps/city_3_orange.png
bitmaps/city_3_red.png
bitmaps/city_3_white.png
bitmaps/city_3_yellow.png
bitmaps/city_4_orange.png
bitmaps/city_4_red.png
bitmaps/city_4_white.png
bitmaps/city_4_yellow.png
bitmaps/cursor_bc.xpm
bitmaps/cursor_bl.xpm
bitmaps/cursor_br.xpm
bitmaps/cursor_cl.xpm
bitmaps/cursor_cr.xpm
bitmaps/cursor_tc.xpm
bitmaps/cursor_tl.xpm
bitmaps/cursor_tr.xpm
bitmaps/mountain_1.png
bitmaps/pole_1.png
bitmaps/pole_2.png
bitmaps/volcano_1.png
bitmaps/border_1.png
bitmaps/border_2.png
bitmaps/river.png
bitmaps/lake.png
bitmaps/topography_0.png
bitmaps/topography_50.png
bitmaps/topography_200.png
bitmaps/topography_500.png
bitmaps/topography_1000.png
bitmaps/topography_2000.png
bitmaps/topography_3500.png
bitmaps/topography_5000.png
bitmaps/topography_7000.png
bitmaps/bathymetry_0.png
bitmaps/bathymetry_-50.png
bitmaps/bathymetry_-200.png
bitmaps/bathymetry_-2000.png
bitmaps/bathymetry_-4000.png
bitmaps/bathymetry_-6500.png
bitmaps/bathymetry_-11000.png
bitmaps/gpsHere.png
DESTINATION ${data_dir}/bitmaps)

install (FILES 
svg/windrose.svg
svg/marble-logo.svg
svg/marble-logo.png
svg/marble-logo-72dpi.png
DESTINATION ${data_dir}/svg)

install (FILES 
flags/flag_ad.svg
flags/flag_ae.svg
flags/flag_af.svg
flags/flag_ag.svg
flags/flag_ai.svg
flags/flag_al.svg
flags/flag_am.svg
flags/flag_an.svg
flags/flag_ao.svg
flags/flag_aq.svg
flags/flag_ar.svg
flags/flag_as.svg
flags/flag_at.svg
flags/flag_au.svg
flags/flag_aw.svg
flags/flag_ax.svg
flags/flag_az.svg
flags/flag_ba.svg
flags/flag_bb.svg
flags/flag_bd.svg
flags/flag_be.svg
flags/flag_bf.svg
flags/flag_bg.svg
flags/flag_bh.svg
flags/flag_bi.svg
flags/flag_bj.svg
flags/flag_bm.svg
flags/flag_bn.svg
flags/flag_bo.svg
flags/flag_br.svg
flags/flag_bs.svg
flags/flag_bt.svg
flags/flag_bv.svg
flags/flag_bw.svg
flags/flag_by.svg
flags/flag_bz.svg
flags/flag_ca.svg
flags/flag_cc.svg
flags/flag_cd.svg
flags/flag_cf.svg
flags/flag_cg.svg
flags/flag_ch.svg
flags/flag_ci.svg
flags/flag_ck.svg
flags/flag_cl.svg
flags/flag_cm.svg
flags/flag_cn.svg
flags/flag_co.svg
flags/flag_cr.svg
flags/flag_cu.svg
flags/flag_cv.svg
flags/flag_cx.svg
flags/flag_cy.svg
flags/flag_cz.svg
flags/flag_de.svg
flags/flag_dj.svg
flags/flag_dk.svg
flags/flag_dm.svg
flags/flag_do.svg
flags/flag_dz.svg
flags/flag_ec.svg
flags/flag_ee.svg
flags/flag_eg.svg
flags/flag_eh.svg
flags/flag_er.svg
flags/flag_es.svg
flags/flag_et.svg
flags/flag_fi.svg
flags/flag_fj.svg
flags/flag_fk.svg
flags/flag_fm.svg
flags/flag_fo.svg
flags/flag_fr.svg
flags/flag_ga.svg
flags/flag_gb.svg
flags/flag_gd.svg
flags/flag_ge.svg
flags/flag_gf.svg
flags/flag_gg.svg
flags/flag_gh.svg
flags/flag_gi.svg
flags/flag_gl.svg
flags/flag_gm.svg
flags/flag_gn.svg
flags/flag_gp.svg
flags/flag_gq.svg
flags/flag_gr.svg
flags/flag_gs.svg
flags/flag_gt.svg
flags/flag_gu.svg
flags/flag_gw.svg
flags/flag_gy.svg
flags/flag_hk.svg
flags/flag_hm.svg
flags/flag_hn.svg
flags/flag_hr.svg
flags/flag_ht.svg
flags/flag_hu.svg
flags/flag_id.svg
flags/flag_ie.svg
flags/flag_il.svg
flags/flag_im.svg
flags/flag_in.svg
flags/flag_io.svg
flags/flag_iq.svg
flags/flag_ir.svg
flags/flag_is.svg
flags/flag_it.svg
flags/flag_je.svg
flags/flag_jm.svg
flags/flag_jo.svg
flags/flag_jp.svg
flags/flag_ke.svg
flags/flag_kg.svg
flags/flag_kh.svg
flags/flag_ki.svg
flags/flag_km.svg
flags/flag_kn.svg
flags/flag_kp.svg
flags/flag_kr.svg
flags/flag_kw.svg
flags/flag_ky.svg
flags/flag_kz.svg
flags/flag_la.svg
flags/flag_lb.svg
flags/flag_lc.svg
flags/flag_li.svg
flags/flag_lk.svg
flags/flag_lr.svg
flags/flag_ls.svg
flags/flag_lt.svg
flags/flag_lu.svg
flags/flag_lv.svg
flags/flag_ly.svg
flags/flag_ma.svg
flags/flag_mc.svg
flags/flag_md.svg
flags/flag_me.svg
flags/flag_mg.svg
flags/flag_mh.svg
flags/flag_mk.svg
flags/flag_ml.svg
flags/flag_mm.svg
flags/flag_mn.svg
flags/flag_mo.svg
flags/flag_mp.svg
flags/flag_mq.svg
flags/flag_mr.svg
flags/flag_ms.svg
flags/flag_mt.svg
flags/flag_mu.svg
flags/flag_mv.svg
flags/flag_mw.svg
flags/flag_mx.svg
flags/flag_my.svg
flags/flag_mz.svg
flags/flag_na.svg
flags/flag_nc.svg
flags/flag_ne.svg
flags/flag_nf.svg
flags/flag_ng.svg
flags/flag_ni.svg
flags/flag_nl.svg
flags/flag_no.svg
flags/flag_np.svg
flags/flag_nr.svg
flags/flag_nu.svg
flags/flag_nz.svg
flags/flag_om.svg
flags/flag_pa.svg
flags/flag_pe.svg
flags/flag_pf.svg
flags/flag_pg.svg
flags/flag_ph.svg
flags/flag_pk.svg
flags/flag_pl.svg
flags/flag_pm.svg
flags/flag_pn.svg
flags/flag_pr.svg
flags/flag_ps.svg
flags/flag_pt.svg
flags/flag_pw.svg
flags/flag_py.svg
flags/flag_qa.svg
flags/flag_re.svg
flags/flag_ro.svg
flags/flag_rs.svg
flags/flag_ru.svg
flags/flag_rw.svg
flags/flag_sa.svg
flags/flag_sb.svg
flags/flag_sc.svg
flags/flag_sd.svg
flags/flag_se.svg
flags/flag_sg.svg
flags/flag_sh.svg
flags/flag_si.svg
flags/flag_sj.svg
flags/flag_sk.svg
flags/flag_sl.svg
flags/flag_sm.svg
flags/flag_sn.svg
flags/flag_so.svg
flags/flag_sr.svg
flags/flag_st.svg
flags/flag_sv.svg
flags/flag_sy.svg
flags/flag_sz.svg
flags/flag_tc.svg
flags/flag_td.svg
flags/flag_tf.svg
flags/flag_tg.svg
flags/flag_th.svg
flags/flag_tj.svg
flags/flag_tk.svg
flags/flag_tl.svg
flags/flag_tm.svg
flags/flag_tn.svg
flags/flag_to.svg
flags/flag_tr.svg
flags/flag_tt.svg
flags/flag_tv.svg
flags/flag_tw.svg
flags/flag_tz.svg
flags/flag_ua.svg
flags/flag_ug.svg
flags/flag_um.svg
flags/flag_us.svg
flags/flag_uy.svg
flags/flag_uz.svg
flags/flag_vc.svg
flags/flag_ve.svg
flags/flag_vg.svg
flags/flag_vi.svg
flags/flag_vn.svg
flags/flag_vu.svg
flags/flag_wf.svg
flags/flag_ws.svg
flags/flag_ye.svg
flags/flag_yt.svg
flags/flag_za.svg
flags/flag_zm.svg
flags/flag_zw.svg
DESTINATION ${data_dir}/flags)

install (FILES
maps/earth/citylights/citylights.jpg
maps/earth/citylights/citylights.dgml
maps/earth/citylights/citylights-preview.png
DESTINATION ${data_dir}/maps/earth/citylights)

install (FILES
maps/earth/srtm/srtm.jpg
maps/earth/srtm/srtm.dgml
maps/earth/srtm/srtm-preview.png
DESTINATION ${data_dir}/maps/earth/srtm)

install (FILES
maps/earth/bluemarble/bluemarble.jpg
maps/earth/bluemarble/bluemarble.dgml
maps/earth/bluemarble/bluemarble-preview.png
DESTINATION ${data_dir}/maps/earth/bluemarble)

install (FILES
mwdbii/PDIFFBORDER.PNT
mwdbii/PUSA48.PNT
mwdbii/PUSA48.DIFF.PNT
mwdbii/PGLACIER.PNT
mwdbii/RIVER.PNT
mwdbii/PCANPROV.PNT
mwdbii/PCOAST.PNT
mwdbii/PISLAND.PNT
mwdbii/PLAKE.PNT
mwdbii/PAUST.PNT
mwdbii/PBORDER.PNT
mwdbii/PMEXICO.PNT
DESTINATION ${data_dir}/mwdbii)

install (FILES
placemarks/baseplacemarks.cache
placemarks/cityplacemarks.cache
placemarks/elevplacemarks.cache
DESTINATION ${data_dir}/placemarks)

