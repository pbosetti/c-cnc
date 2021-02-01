-- Machine tool configuration file
-- Mandatory fields for each axis:
--     machine.x = {mass=100, res=10}
--     machine.x.pid = {kp=1, ki=0, kd=0, saturation={0,0}}
--  see http://support.motioneng.com/downloads-notes/tuning/pid_overshoot.htm
machine = {}

machine.cfg = {A=100, D=120, tq=0.005, error=0.05}
machine.cfg.zero = {0, 0, 400}

machine.x = {mass=2000, res=50000}
machine.x.pid = {kp=2500, ki=0.01, kd=5.1, saturation={-5000,5000}}

machine.y = {mass=1200, res=30000}
machine.y.pid = {kp=2300, ki=0.01, kd=5, saturation={-5000,5000}}

machine.z = {mass=700, res=30000}
machine.z.pid = {kp=1800, ki=0.01, kd=3.5, saturation={-2500,2500}}
