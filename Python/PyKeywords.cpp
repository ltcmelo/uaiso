/******************************************************************************
 * Copyrightc) 2014-2015 Leandro T. C. Melo (ltcmelo@gmail.com)
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

#include "Python/PyKeywords.h"

using namespace uaiso;

Token PyKeywords::classify(const char* s, size_t length)
{
    if (length == 2)
        return classify2(s);
    if (length == 3)
        return classify3(s);
    if (length == 4)
        return classify4(s);
    if (length == 5)
        return classify5(s);
    if (length == 6)
        return classify6(s);
    if (length == 7)
        return classify7(s);
    if (length == 8)
        return classify8(s);

    return TK_IDENTIFIER;
}

Token PyKeywords::classify2(const char* s)
{
    if (s[0] == 'a') {
        if (s[1] == 's')
            return TK_AS;
    } else if (s[0] == 'i') {
        if (s[1] == 'f')
            return TK_IF;
        if (s[1] == 'n')
            return TK_IN;
        if (s[1] == 's')
            return TK_IS;
    } else if (s[0] == 'o') {
        if (s[1] == 'r')
            return TK_OR;
    }

    return TK_IDENTIFIER;
}

Token PyKeywords::classify3(const char* s)
{
    if (s[0] == 'a') {
        if (s[1] == 'n') {
            if (s[2] == 'd')
                return TK_AND;
        }
    } else if (s[0] == 'd') {
        if (s[1] == 'e') {
            if (s[2] == 'f')
                return TK_FUNC;
            if (s[2] == 'l')
                return TK_DELETE;
        }
    } else if (s[0] == 'f') {
        if (s[1] == 'o') {
            if (s[2] == 'r')
                return TK_FOR;
        }
    } else if (s[0] == 'n') {
        if (s[1] == 'o') {
            if (s[2] == 't')
                return TK_NOT;
        }
    } else if (s[0] == 't') {
        if (s[1] == 'r') {
            if (s[2] == 'y')
                return TK_TRY;
        }
    }

    return TK_IDENTIFIER;
}

Token PyKeywords::classify4(const char* s)
{
    if (s[0] == 'N') {
        if (s[1] == 'o') {
            if (s[2] == 'n') {
                if (s[3] == 'e')
                    return TK_NULL_LITERAL;
            }
        }
    } else if (s[0] == 'T') {
        if (s[1] == 'r') {
            if (s[2] == 'u') {
                if (s[3] == 'e')
                    return TK_TRUE_LITERAL;
            }
        }
    } else if (s[0] == 'e') {
        if (s[1] == 'l') {
            if (s[2] == 'i') {
                if (s[3] == 'f')
                    return TK_ELIF;
            } else if (s[2] == 's') {
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
    } else if (s[0] == 'p') {
        if (s[1] == 'a') {
            if (s[2] == 's') {
                if (s[3] == 's')
                    return TK_PASS;
            }
        }
    } else if (s[0] == 'w') {
        if (s[1] == 'i') {
            if (s[2] == 't') {
                if (s[3] == 'h')
                    return TK_WITH;
            }
        }
    }

    return TK_IDENTIFIER;
}

Token PyKeywords::classify5(const char* s)
{
    if (s[0] == 'F') {
        if (s[1] == 'a') {
            if (s[2] == 'l') {
                if (s[3] == 's') {
                    if (s[4] == 'e')
                        return TK_FALSE_LITERAL;
                }
            }
        }
    } else if (s[0] == 'b') {
        if (s[1] == 'r') {
            if (s[2] == 'e') {
                if (s[3] == 'a') {
                    if (s[4] == 'k')
                        return TK_BREAK;
                }
            }
        }
    } else if (s[0] == 'c') {
        if (s[1] == 'l') {
            if (s[2] == 'a') {
                if (s[3] == 's') {
                    if (s[4] == 's')
                        return TK_CLASS;
                }
            }
        }
    } else if (s[0] == 'r') {
        if (s[1] == 'a') {
            if (s[2] == 'i') {
                if (s[3] == 's') {
                    if (s[4] == 'e')
                        return TK_THROW;
                }
            }
        }
    } else if (s[0] == 'w') {
        if (s[1] == 'h') {
            if (s[2] == 'i') {
                if (s[3] == 'l') {
                    if (s[4] == 'e')
                        return TK_WHILE;
                }
            }
        }
    } else if (s[0] == 'y') {
        if (s[1] == 'i') {
            if (s[2] == 'e') {
                if (s[3] == 'l') {
                    if (s[4] == 'd')
                        return TK_YIELD;
                }
            }
        }
    }

    return TK_IDENTIFIER;
}

Token PyKeywords::classify6(const char* s)
{
    if (s[0] == 'a') {
        if (s[1] == 's') {
            if (s[2] == 's') {
                if (s[3] == 'e') {
                    if (s[4] == 'r') {
                        if (s[5] == 't')
                            return TK_ASSERT;
                    }
                }
            }
        }
    } else if (s[0] == 'e') {
        if (s[1] == 'x') {
            if (s[2] == 'c') {
                if (s[3] == 'e') {
                    if (s[4] == 'p') {
                        if (s[5] == 't')
                            return TK_CATCH;
                    }
                }
            }
        }
    } else if (s[0] == 'g') {
        if (s[1] == 'l') {
            if (s[2] == 'o') {
                if (s[3] == 'b') {
                    if (s[4] == 'a') {
                        if (s[5] == 'l')
                            return TK_GLOBAL;
                    }
                }
            }
        }
    } else if (s[0] == 'i') {
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
    } else if (s[0] == 'l') {
        if (s[1] == 'a') {
            if (s[2] == 'm') {
                if (s[3] == 'b') {
                    if (s[4] == 'd') {
                        if (s[5] == 'a')
                            return TK_LAMBDA;
                    }
                }
            }
        }
    } else if (s[0] == 'r') {
        if (s[1] == 'e') {
            if (s[2] == 't') {
                if (s[3] == 'u') {
                    if (s[4] == 'r') {
                        if (s[5] == 'n')
                            return TK_RETURN;
                    }
                }
            }
        }
    }

    return TK_IDENTIFIER;
}

Token PyKeywords::classify7(const char* s)
{
    if (s[0] == 'f') {
        if (s[1] == 'i') {
            if (s[2] == 'n') {
                if (s[3] == 'a') {
                    if (s[4] == 'l') {
                        if (s[5] == 'l') {
                            if (s[6] == 'y')
                                return TK_FINALLY;
                        }
                    }
                }
            }
        }
    }

    return TK_IDENTIFIER;
}

Token PyKeywords::classify8(const char* s)
{
    if (s[0] == 'c') {
        if (s[1] == 'o') {
            if (s[2] == 'n') {
                if (s[3] == 't') {
                    if (s[4] == 'i') {
                        if (s[5] == 'n') {
                            if (s[6] == 'u') {
                                if (s[7] == 'e')
                                    return TK_CONTINUE;
                            }
                        }
                    }
                }
            }
        }
    } else if (s[0] == 'n') {
        if (s[1] == 'o') {
            if (s[2] == 'n') {
                if (s[3] == 'l') {
                    if (s[4] == 'o') {
                        if (s[5] == 'c') {
                            if (s[6] == 'a') {
                                if (s[7] == 'l')
                                    return TK_NONLOCAL;
                            }
                        }
                    }
                }
            }
        }
    }

    return TK_IDENTIFIER;
}
