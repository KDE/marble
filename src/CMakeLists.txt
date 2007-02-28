project(marble)

option(QTONLY  "Build Qt only")

find_package(Qt4 REQUIRED)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

if (NOT QTONLY)
  # search packages used by KDE
  find_package(KDE4 REQUIRED)
  include (KDE4Defaults)
  include (MacroLibrary)
endif (NOT QTONLY)


if (QTONLY)
  add_definitions (${QT_DEFINITIONS} -msse)
  include_directories (${CMAKE_BUILD_DIR} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${QT_INCLUDE_DIR})
else (QTONLY)
  add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -msse -DKDEBUILD)
  include_directories (${CMAKE_BUILD_DIR} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})
endif (QTONLY)

link_directories (${QT_LIBRARY_DIR})

set(marble_SRCS
           katlascontrol.cpp
           katlasview.cpp
           katlasviewinputhandler.cpp
           main.cpp
           texcolorizer.cpp
           texloader.cpp
           texmapper.cpp
           quaternion.cpp
           katlasglobe.cpp
           katlastoolbox.cpp
           pntmap.cpp
           vectorcomposer.cpp
           vectormap.cpp
           clippainter.cpp
           geopoint.cpp
           placemarkmanager.cpp
           placecontainer.cpp
           placemarkstorage.cpp
           placefolder.cpp
           placemark.cpp
           searchlistview.cpp
           katlasmapscale.cpp
           placemarkmodel.cpp
           placemarkpainter.cpp
           maptheme.cpp
           tilescissor.cpp
           katlastilecreatordialog.cpp
           katlasthemeselectview.cpp
           katlaswindrose.cpp
           katlasviewpopupmenu.cpp
           katlascrosshair.cpp
           xmlhandler.cpp
           placemarkinfodialog.cpp
           katlasflag.cpp
           gridmap.cpp
           tinywebbrowser.cpp
           httpfetchfile.cpp
)

set (marble_UI katlastilecreatordialog.ui
	   katlastoolbox.ui placemarkinfodialog.ui)

qt4_add_resources(marble_SRCS marble.qrc)

if (QTONLY)
  qt4_wrap_ui(marble_SRCS ${marble_UI})
  qt4_automoc(${marble_SRCS})
else (QTONLY)
  kde4_add_ui_files(marble_SRCS ${marble_UI})
  kde4_automoc(${marble_SRCS})
endif (QTONLY)

#kde4_add_library (marble SHARED ${marble_SRCS})
if (QTONLY)
  add_executable (marble ${marble_SRCS})
else (QTONLY)
  kde4_add_executable (marble ${marble_SRCS})
endif (QTONLY)

if (QTONLY)
	target_link_libraries (marble ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTSVG_LIBRARY} ${QT_QTNETWORK_LIBRARY})
else (QTONLY)
	target_link_libraries (marble ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTSVG_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${KDE4_KDECORE_LIBRARY})
endif (QTONLY)

#set_target_properties(marble PROPERTIES VERSION 1.0.0 SOVERSION 1)
install (TARGETS marble DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
#install (TARGETS marble DESTINATION ${LIB_INSTALL_DIR} )

install (FILES
../data/bitmaps/city_1_orange.png
../data/bitmaps/city_1_red.png
../data/bitmaps/city_1_white.png
../data/bitmaps/city_1_yellow.png
../data/bitmaps/city_2_orange.png
../data/bitmaps/city_2_red.png
../data/bitmaps/city_2_white.png
../data/bitmaps/city_2_yellow.png
../data/bitmaps/city_3_orange.png
../data/bitmaps/city_3_red.png
../data/bitmaps/city_3_white.png
../data/bitmaps/city_3_yellow.png
../data/bitmaps/city_4_orange.png
../data/bitmaps/city_4_red.png
../data/bitmaps/city_4_white.png
../data/bitmaps/city_4_yellow.png
../data/bitmaps/cursor_bc.xpm
../data/bitmaps/cursor_bl.xpm
../data/bitmaps/cursor_br.xpm
../data/bitmaps/cursor_cl.xpm
../data/bitmaps/cursor_cr.xpm
../data/bitmaps/cursor_tc.xpm
../data/bitmaps/cursor_tl.xpm
../data/bitmaps/cursor_tr.xpm
../data/bitmaps/mountain_1.png
../data/bitmaps/pole_1.png
../data/bitmaps/pole_2.png
../data/bitmaps/volcano_1.png
DESTINATION ${CMAKE_INSTALL_PREFIX}/data/bitmaps)

install (FILES 
../data/svg/windrose.svg
DESTINATION ${CMAKE_INSTALL_PREFIX}/data/svg)

install (FILES 
../data/flags/flag_ad.svg
../data/flags/flag_ae.svg
../data/flags/flag_af.svg
../data/flags/flag_ag.svg
../data/flags/flag_ai.svg
../data/flags/flag_al.svg
../data/flags/flag_am.svg
../data/flags/flag_an.svg
../data/flags/flag_ao.svg
../data/flags/flag_aq.svg
../data/flags/flag_ar.svg
../data/flags/flag_as.svg
../data/flags/flag_at.svg
../data/flags/flag_au.svg
../data/flags/flag_aw.svg
../data/flags/flag_ax.svg
../data/flags/flag_az.svg
../data/flags/flag_ba.svg
../data/flags/flag_bb.svg
../data/flags/flag_bd.svg
../data/flags/flag_be.svg
../data/flags/flag_bf.svg
../data/flags/flag_bg.svg
../data/flags/flag_bh.svg
../data/flags/flag_bi.svg
../data/flags/flag_bj.svg
../data/flags/flag_bm.svg
../data/flags/flag_bn.svg
../data/flags/flag_bo.svg
../data/flags/flag_br.svg
../data/flags/flag_bs.svg
../data/flags/flag_bt.svg
../data/flags/flag_bv.svg
../data/flags/flag_bw.svg
../data/flags/flag_by.svg
../data/flags/flag_bz.svg
../data/flags/flag_ca.svg
../data/flags/flag_cc.svg
../data/flags/flag_cd.svg
../data/flags/flag_cf.svg
../data/flags/flag_cg.svg
../data/flags/flag_ch.svg
../data/flags/flag_ci.svg
../data/flags/flag_ck.svg
../data/flags/flag_cl.svg
../data/flags/flag_cm.svg
../data/flags/flag_cn.svg
../data/flags/flag_co.svg
../data/flags/flag_cr.svg
../data/flags/flag_cu.svg
../data/flags/flag_cv.svg
../data/flags/flag_cx.svg
../data/flags/flag_cy.svg
../data/flags/flag_cz.svg
../data/flags/flag_de.svg
../data/flags/flag_dj.svg
../data/flags/flag_dk.svg
../data/flags/flag_dm.svg
../data/flags/flag_do.svg
../data/flags/flag_dz.svg
../data/flags/flag_ec.svg
../data/flags/flag_ee.svg
../data/flags/flag_eg.svg
../data/flags/flag_eh.svg
../data/flags/flag_er.svg
../data/flags/flag_es.svg
../data/flags/flag_et.svg
../data/flags/flag_fi.svg
../data/flags/flag_fj.svg
../data/flags/flag_fk.svg
../data/flags/flag_fm.svg
../data/flags/flag_fo.svg
../data/flags/flag_fr.svg
../data/flags/flag_ga.svg
../data/flags/flag_gb.svg
../data/flags/flag_gd.svg
../data/flags/flag_ge.svg
../data/flags/flag_gf.svg
../data/flags/flag_gg.svg
../data/flags/flag_gh.svg
../data/flags/flag_gi.svg
../data/flags/flag_gl.svg
../data/flags/flag_gm.svg
../data/flags/flag_gn.svg
../data/flags/flag_gp.svg
../data/flags/flag_gq.svg
../data/flags/flag_gr.svg
../data/flags/flag_gs.svg
../data/flags/flag_gt.svg
../data/flags/flag_gu.svg
../data/flags/flag_gw.svg
../data/flags/flag_gy.svg
../data/flags/flag_hk.svg
../data/flags/flag_hm.svg
../data/flags/flag_hn.svg
../data/flags/flag_hr.svg
../data/flags/flag_ht.svg
../data/flags/flag_hu.svg
../data/flags/flag_id.svg
../data/flags/flag_ie.svg
../data/flags/flag_il.svg
../data/flags/flag_im.svg
../data/flags/flag_in.svg
../data/flags/flag_io.svg
../data/flags/flag_iq.svg
../data/flags/flag_ir.svg
../data/flags/flag_is.svg
../data/flags/flag_it.svg
../data/flags/flag_je.svg
../data/flags/flag_jm.svg
../data/flags/flag_jo.svg
../data/flags/flag_jp.svg
../data/flags/flag_ke.svg
../data/flags/flag_kg.svg
../data/flags/flag_kh.svg
../data/flags/flag_ki.svg
../data/flags/flag_km.svg
../data/flags/flag_kn.svg
../data/flags/flag_kp.svg
../data/flags/flag_kr.svg
../data/flags/flag_kw.svg
../data/flags/flag_ky.svg
../data/flags/flag_kz.svg
../data/flags/flag_la.svg
../data/flags/flag_lb.svg
../data/flags/flag_lc.svg
../data/flags/flag_li.svg
../data/flags/flag_lk.svg
../data/flags/flag_lr.svg
../data/flags/flag_ls.svg
../data/flags/flag_lt.svg
../data/flags/flag_lu.svg
../data/flags/flag_lv.svg
../data/flags/flag_ly.svg
../data/flags/flag_ma.svg
../data/flags/flag_mc.svg
../data/flags/flag_md.svg
../data/flags/flag_me.svg
../data/flags/flag_mg.svg
../data/flags/flag_mh.svg
../data/flags/flag_mk.svg
../data/flags/flag_ml.svg
../data/flags/flag_mm.svg
../data/flags/flag_mn.svg
../data/flags/flag_mo.svg
../data/flags/flag_mp.svg
../data/flags/flag_mq.svg
../data/flags/flag_mr.svg
../data/flags/flag_ms.svg
../data/flags/flag_mt.svg
../data/flags/flag_mu.svg
../data/flags/flag_mv.svg
../data/flags/flag_mw.svg
../data/flags/flag_mx.svg
../data/flags/flag_my.svg
../data/flags/flag_mz.svg
../data/flags/flag_na.svg
../data/flags/flag_nc.svg
../data/flags/flag_ne.svg
../data/flags/flag_nf.svg
../data/flags/flag_ng.svg
../data/flags/flag_ni.svg
../data/flags/flag_nl.svg
../data/flags/flag_no.svg
../data/flags/flag_np.svg
../data/flags/flag_nr.svg
../data/flags/flag_nu.svg
../data/flags/flag_nz.svg
../data/flags/flag_om.svg
../data/flags/flag_pa.svg
../data/flags/flag_pe.svg
../data/flags/flag_pf.svg
../data/flags/flag_pg.svg
../data/flags/flag_ph.svg
../data/flags/flag_pk.svg
../data/flags/flag_pl.svg
../data/flags/flag_pm.svg
../data/flags/flag_pn.svg
../data/flags/flag_pr.svg
../data/flags/flag_ps.svg
../data/flags/flag_pt.svg
../data/flags/flag_pw.svg
../data/flags/flag_py.svg
../data/flags/flag_qa.svg
../data/flags/flag_re.svg
../data/flags/flag_ro.svg
../data/flags/flag_rs.svg
../data/flags/flag_ru.svg
../data/flags/flag_rw.svg
../data/flags/flag_sa.svg
../data/flags/flag_sb.svg
../data/flags/flag_sc.svg
../data/flags/flag_sd.svg
../data/flags/flag_se.svg
../data/flags/flag_sg.svg
../data/flags/flag_sh.svg
../data/flags/flag_si.svg
../data/flags/flag_sj.svg
../data/flags/flag_sk.svg
../data/flags/flag_sl.svg
../data/flags/flag_sm.svg
../data/flags/flag_sn.svg
../data/flags/flag_so.svg
../data/flags/flag_sr.svg
../data/flags/flag_st.svg
../data/flags/flag_sv.svg
../data/flags/flag_sy.svg
../data/flags/flag_sz.svg
../data/flags/flag_tc.svg
../data/flags/flag_td.svg
../data/flags/flag_tf.svg
../data/flags/flag_tg.svg
../data/flags/flag_th.svg
../data/flags/flag_tj.svg
../data/flags/flag_tk.svg
../data/flags/flag_tl.svg
../data/flags/flag_tm.svg
../data/flags/flag_tn.svg
../data/flags/flag_to.svg
../data/flags/flag_tr.svg
../data/flags/flag_tt.svg
../data/flags/flag_tv.svg
../data/flags/flag_tw.svg
../data/flags/flag_tz.svg
../data/flags/flag_ua.svg
../data/flags/flag_ug.svg
../data/flags/flag_um.svg
../data/flags/flag_us.svg
../data/flags/flag_uy.svg
../data/flags/flag_uz.svg
../data/flags/flag_vc.svg
../data/flags/flag_ve.svg
../data/flags/flag_vg.svg
../data/flags/flag_vi.svg
../data/flags/flag_vn.svg
../data/flags/flag_vu.svg
../data/flags/flag_wf.svg
../data/flags/flag_ws.svg
../data/flags/flag_ye.svg
../data/flags/flag_yt.svg
../data/flags/flag_za.svg
../data/flags/flag_zm.svg
../data/flags/flag_zw.svg
DESTINATION ${CMAKE_INSTALL_PREFIX}/data/flags)

install (FILES
../data/maps/citylights/citylights.jpg
../data/maps/citylights/citylights.dgml
../data/maps/citylights/citylights-preview.png
DESTINATION ${CMAKE_INSTALL_PREFIX}/data/maps/citylights)

install (FILES
../data/maps/etopo2/etopo.jpg
../data/maps/etopo2/etopo2.dgml
../data/maps/etopo2/etopo-preview.png
DESTINATION ${CMAKE_INSTALL_PREFIX}/data/maps/etopo2)

install (FILES
../data/maps/bluemarble/bluemarble.jpg
../data/maps/bluemarble/bluemarble.dgml
../data/maps/bluemarble/bluemarble-preview.png
DESTINATION ${CMAKE_INSTALL_PREFIX}/data/maps/bluemarble)

install (FILES
../data/vectorsource/glaciers.svg
../data/vectorsource/islands.svg
../data/vectorsource/lakes.svg
DESTINATION ${CMAKE_INSTALL_PREFIX}/data/vectorsource)

install (FILES
../data/mwdbii/PDIFFBORDER.PNT
../data/mwdbii/PUSA48.PNT
../data/mwdbii/PGLACIER.PNT
../data/mwdbii/PISLAND.PNT.orig
../data/mwdbii/RIVER.PNT
../data/mwdbii/PCANPROV.PNT
../data/mwdbii/PUSA48.DIFF.PNT
../data/mwdbii/PCOAST.PNT
../data/mwdbii/PISLAND.PNT
../data/mwdbii/PLAKE.PNT.orig
../data/mwdbii/Pntinfo.txt
../data/mwdbii/PLAKE.PNT
../data/mwdbii/PAUST.PNT
../data/mwdbii/PBORDER.PNT
../data/mwdbii/PMEXICO.PNT
../data/mwdbii/GOODEIHS.BAS
../data/mwdbii/PBORDER.TXT
../data/mwdbii/MWDBPOLY.TXT
../data/mwdbii/contents.txt
../data/mwdbii/GLOBAL.TXT
DESTINATION ${CMAKE_INSTALL_PREFIX}/data/mwdbii)

install (FILES
../data/placemarks/baseplacemarks.cache
../data/placemarks/cityplacemarks.cache
../data/placemarks/elevplacemarks.cache
DESTINATION ${CMAKE_INSTALL_PREFIX}/data/placemarks)

install ( FILES
../data/seacolors.leg
../data/landcolors.leg
DESTINATION ${CMAKE_INSTALL_PREFIX}/data)


install (FILES  DESTINATION ${INCLUDE_INSTALL_DIR} )
install( FILES marble.desktop  DESTINATION ${XDG_APPS_DIR})
