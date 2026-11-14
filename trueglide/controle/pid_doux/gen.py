import re,sys
S=sys.argv[1]
tg=open(S+'/sauv/MINI_POUCE_TRUEGLIDE_diff_all_2026-11-08_cb4e3f0.txt').read().splitlines()
xp=open('/root/.claude/uploads/ffb92c57-b666-5b6d-adb7-c1982cccd2cb/4c688d90-MINI_POUCE_XPro_diff_all_PID_doux_2026-11-09.txt').read().splitlines()
blocks={}; cur=None
for l in xp:
    m=re.match(r'(profile|rateprofile) (\d+)$',l)
    if m:
        cur=(m.group(1),int(m.group(2))); blocks.setdefault(cur,{}); continue
    m=re.match(r'set (\w+) = (.*)$',l)
    if m and cur: blocks[cur][m.group(1)]=m.group(2)
KEEP_TG={'profile_name','launch_control_mode','launch_trigger_allow_reset','launch_trigger_throttle_percent','launch_angle_limit','launch_control_gain','dyn_idle_min_rpm','dyn_idle_p_gain','dyn_idle_i_gain','dyn_idle_d_gain','dyn_idle_max_increase','thrust_linear','auto_profile_cell_count','rateprofile_name'}
out=[]; cur=None; done=set()
def flush():
    if cur and cur in blocks and cur not in done:
        done.add(cur)
        for k,v in blocks[cur].items():
            if k not in KEEP_TG: out.append(f'set {k} = {v}')
for l in tg:
    m=re.match(r'(profile|rateprofile) (\d+)$',l)
    if m:
        flush(); cur=(m.group(1),int(m.group(2))); out.append(l); continue
    if re.match(r'(battery_profile|# restore|# save|save$)',l):
        flush(); cur=None; out.append(l); continue
    mm=re.match(r'set (\w+) = ',l)
    if cur and cur in blocks and cur not in done and mm and mm.group(1) not in KEEP_TG: continue
    out.append(l)
hdr=['# Mini Pouce, TRUEGLIDE cb4e3f0 : sauvegarde du 08/11 avec les PID et rates « doux » de la sauvegarde XPro du 09/11.',
     '# Profils PID 0 à 3 et profils de rates 0 à 3 : valeurs XPro. Gardés du 08/11 : noms, launch control, ralenti dynamique,',
     '# thrust_linear, choix du profil selon la batterie, et tout le reste (modes, OSD, ports, filtres gyro, airmode, small_angle).',
     "# NE PAS COLLER avant le OUI de l'audit et le vol TG-P3B."]
open(S+'/piddoux/MINI_POUCE_TRUEGLIDE_PID_doux_cb4e3f0.txt','w').write('\n'.join(hdr+out)+'\n')
print(sorted(done))
