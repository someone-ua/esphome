import esphome.codegen as cg
from esphome.components import climate_ir

AUTO_LOAD = ["climate_ir"]

electra_ac__ns = cg.esphome_ns.namespace("electra_ac")
ElectraClimate = zhlt01_ns.class_("ElectraClimate", climate_ir.ClimateIR)

CONFIG_SCHEMA = climate_ir.climate_ir_with_receiver_schema(ElectraClimate)


async def to_code(config):
    await climate_ir.new_climate_ir(config)
