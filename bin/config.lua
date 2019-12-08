-- Machine tool configuration file
-- Mandatory fields for each axis:
--     machine.x = {mass=100, res=10}
--     machine.x.pid = {kp=1, ki=0, kd=0, saturation={0,0}}
--  see http://support.motioneng.com/downloads-notes/tuning/pid_overshoot.htm
machine = {}

machine.cfg = {A=10, D=50, tq=0.005, error=0.005}
machine.cfg.zero = {0, 0, 400}

machine.x = {mass=2000, res=50000}
machine.x.pid = {kp=7.5, ki=0.001, kd=1.25 * 2/3, saturation={-5000,5000}}

machine.y = {mass=1200, res=30000}
machine.y.pid = {kp=13, ki=0, kd=0.7*2/3, saturation={-5000,5000}}

machine.z = {mass=700, res=30000}
machine.z.pid = {kp=5, ki=0.1, kd=1*2/3, saturation={-2500,2500}}
