#include "scalyc.h"

_scalyc_Options_Result_invalidOption* _new_scalyc_Options_Result_invalidOption(scaly_Page* _page, scaly_string* option) {
    _scalyc_Options_Result_invalidOption* this = (_scalyc_Options_Result_invalidOption*) scaly_Page_allocateObject(_page, sizeof(_scalyc_Options_Result_invalidOption));
    this->option = option;
    return this;
}

_scalyc_Options_Result_unknownOption* _new_scalyc_Options_Result_unknownOption(scaly_Page* _page, scaly_string* option) {
    _scalyc_Options_Result_unknownOption* this = (_scalyc_Options_Result_unknownOption*) scaly_Page_allocateObject(_page, sizeof(_scalyc_Options_Result_unknownOption));
    this->option = option;
    return this;
}