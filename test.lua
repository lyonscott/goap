local goap=require "goap"
local ctx=goap.mk_context()
local ipairs=ipairs

local idle={
    condition={
        idleable=true,
    },
    effect={
        attackable=true,
    },
    cost=5,
}
local attack={
    condition={
        attackable=true,
        target=true,
    },
    effect={
        killable=true,
        spellable=true,
    },
    cost=2,
}
local spell={
    condition={
        target=true,
        spellable=true,
    },
    effect={
        spellable=false,
        killable=true,
    },
    cost=2,
}
local search={
    condition={
        target=false,
    },
    effect={
        target=true,
    },
    cost=1,
}
local hide={
    condition={
        lowhp=true,
    },
    effect={
        lowhp=false,
    },
    cost=4,
}
goap.set_action(ctx,"idle",idle)
goap.set_action(ctx,"attack",attack)
goap.set_action(ctx,"spell",spell)
goap.set_action(ctx,"search",search)
goap.set_action(ctx,"hide",hide)
--[[
local datalist=require "datalist"
local pairs=pairs

local data=load_file("context.txt")
local ctx_data=datalist.parse(data)
for k,v in pairs(ctx_data) do
    goap.set_action(ctx,k,v)
end
]]
local state={
    stun=false,
    idleable=true,
    attackable=false,
    spellable=true,
    killable=false,
    target=false,
}
local goal={
    killable=true,
}
local plans=goap.plan(ctx,state,goal)
for k,v in ipairs(plans) do
    print(k,v)
end