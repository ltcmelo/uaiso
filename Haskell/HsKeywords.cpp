/******************************************************************************
 * Copyright (c) 2014-2016 Leandro T. C. Melo (ltcmelo@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 *****************************************************************************/

/*--------------------------*/
/*--- The UaiSo! Project ---*/
/*--------------------------*/

#include "Haskell/HsKeywords.h"

using namespace uaiso;

Token HsKeywords::filter(const char* s, size_t len)
{
    if (len == 1)
        return filter1(s);
    if (len == 2)
        return filter2(s);
    if (len == 3)
        return filter3(s);
    if (len == 4)
        return filter4(s);
    if (len == 5)
        return filter5(s);
    if (len == 6)
        return filter6(s);
    if (len == 7)
        return filter7(s);
    if (len == 8)
        return filter8(s);

    return TK_INVALID;
}

Token HsKeywords::filter1(const char* s)
{
    if (s[0] == '_')
        return TK_UNDERSCORE;

    return TK_INVALID;
}

Token HsKeywords::filter2(const char* s)
{
    if (s[0] == 'd') {
        if (s[1] == 'o')
            return TK_DO;
    } else if (s[0] == 'i') {
        if (s[1] == 'f')
            return TK_IF;
        if (s[1] == 'n')
            return TK_IN;
    } else if (s[0] == 'o') {
        if (s[1] == 'f')
            return TK_OF;
    }

    return TK_INVALID;
}

Token HsKeywords::filter3(const char* s)
{
    if (s[0] == 'l') {
        if (s[1] == 'e') {
            if (s[2] == 't')
                return TK_LET;
        }
    }

    return TK_INVALID;
}

Token HsKeywords::filter4(const char* s)
{
    if (s[0] == 'c') {
        if (s[1] == 'a') {
            if (s[2] == 's') {
                if (s[3] == 'e')
                    return TK_CASE;
            }
        }
    } else if (s[0] == 'd') {
        if (s[1] == 'a') {
            if (s[2] == 't') {
                if (s[3] == 'a')
                    return TK_DATA;
            }
        }
    } else if (s[0] == 'e') {
        if (s[1] == 'l') {
            if (s[2] == 's') {
                if (s[3] == 'e')
                    return TK_ELSE;
            }
        }
    } else if (s[0] == 'f') {
        if (s[1] == 'r') {
            if (s[2] == 'o') {
                if (s[3] == 'm')
                    return TK_FROM;
            }
        }
    } else if (s[0] == 't') {
        if (s[1] == 'h') {
            if (s[2] == 'e') {
                if (s[3] == 'n')
                    return TK_THEN;
            }
        } else if (s[1] == 'y') {
            if (s[2] == 'p') {
                if (s[3] == 'e')
                    return TK_TYPE;
            }
        }
    }

    return TK_INVALID;
}

Token HsKeywords::filter5(const char* s)
{
    if (s[0] == 'c') {
        if (s[1] == 'l') {
            if (s[2] == 'a') {
                if (s[3] == 's') {
                    if (s[4] == 's')
                        return TK_CLASS;
                }
            }
        }
    } else if (s[0] == 'i') {
        if (s[1] == 'n') {
            if (s[2] == 'f') {
                if (s[3] == 'i') {
                    if (s[4] == 'x')
                        return TK_INFIX;
                }
            }
        }
    } else if (s[0] == 'w') {
        if (s[1] == 'h') {
            if (s[2] == 'e') {
                if (s[3] == 'r') {
                    if (s[4] == 'e')
                        return TK_WHERE;
                }
            }
        }
    }

    return TK_INVALID;
}

Token HsKeywords::filter6(const char* s)
{
     if (s[0] == 'i') {
        if (s[1] == 'm') {
            if (s[2] == 'p') {
                if (s[3] == 'o') {
                    if (s[4] == 'r') {
                        if (s[5] == 't')
                            return TK_IMPORT;
                    }
                }
            }
        }
    } else if (s[0] == 'i') {
         if (s[1] == 'n') {
             if (s[2] == 'f') {
                 if (s[3] == 'i') {
                     if (s[4] == 'x') {
                         if (s[5] == 'r')
                             return TK_INFIXR;
                         if (s[5] == 'l')
                             return TK_INFIXL;
                     }
                 }
             }
         }
     } else if (s[0] == 'm') {
        if (s[1] == 'o') {
            if (s[2] == 'd') {
                if (s[3] == 'u') {
                    if (s[4] == 'l') {
                        if (s[5] == 'e')
                            return TK_MODULE;
                    }
                }
            }
        }
    }

    return TK_INVALID;
}

Token HsKeywords::filter7(const char* s)
{
    if (s[0] == 'd') {
        if (s[1] == 'e') {
            if (s[2] == 'f') {
                if (s[3] == 'a') {
                    if (s[4] == 'u') {
                        if (s[5] == 'l') {
                            if (s[6] == 't')
                                return TK_DEFAULT;
                        }
                    }
                }
            }
        }
    } else if (s[0] == 'f') {
        if (s[1] == 'o') {
            if (s[2] == 'r') {
                if (s[3] == 'e') {
                    if (s[4] == 'i') {
                        if (s[5] == 'g') {
                            if (s[6] == 'n')
                                return TK_FOREIGN;
                        }
                    }
                }
            }
        }
    } else if (s[0] == 'n') {
        if (s[1] == 'e') {
            if (s[2] == 'w') {
                if (s[3] == 't') {
                    if (s[4] == 'y') {
                        if (s[5] == 'p') {
                            if (s[6] == 'e')
                                return TK_NEWTYPE;
                        }
                    }
                }
            }
        }
    }

    return TK_INVALID;
}

Token HsKeywords::filter8(const char* s)
{
    if (s[0] == 'd') {
        if (s[1] == 'e') {
            if (s[2] == 'r') {
                if (s[3] == 'i') {
                    if (s[4] == 'v') {
                        if (s[5] == 'i') {
                            if (s[6] == 'n') {
                                if (s[7] == 'g')
                                    return TK_DERIVING;
                            }
                        }
                    }
                }
            }
        }
    } else if (s[0] == 'i') {
        if (s[1] == 'n') {
            if (s[2] == 's') {
                if (s[3] == 't') {
                    if (s[4] == 'a') {
                        if (s[5] == 'n') {
                            if (s[6] == 'c') {
                                if (s[7] == 'e')
                                    return TK_INSTANCE;
                            }
                        }
                    }
                }
            }
        }
    }

    return TK_INVALID;
}
