skill = {
    audio: 2,
    enable: ["chooseToUse", "chooseToRespond"],
    prompt: "将♦牌当做杀，♥牌当做桃，♣牌当做闪，♠牌当做无懈可击使用或打出",
    viewAs: function (cards, player) {
        var name = false;
        var nature = null;
        //根据选择的卡牌的花色 判断要转化出的卡牌是闪还是火杀还是无懈还是桃
        switch (get.suit(cards[0], player)) {
            case 'club': name = 'shan'; break;
            case 'diamond': name = 'sha'; nature = 'fire'; break;
            case 'spade': name = 'wuxie'; break;
            case 'heart': name = 'tao'; break;
        }
        //返回判断结果
        if (name) return { name: name, nature: nature };
        return null;
    },
    check: function (card) {
        if (ui.selected.cards.length) return 0;
        var player = _status.event.player;
        if (_status.event.type == 'phase') {
            var max = 0;
            var name2;
            var list = ['sha', 'tao'];
            var map = { sha: 'diamond', tao: 'heart' }
            for (var i = 0; i < list.length; i++) {
                var name = list[i];
                if (player.countCards('hes', function (card) {
                    return (name != 'sha' || get.value(card) < 5) && get.suit(card, player) == map[name];
                }) > 0 && player.getUseValue({ name: name, nature: name == 'sha' ? 'fire' : null }) > 0) {
                    var temp = get.order({ name: name, nature: name == 'sha' ? 'fire' : null });
                    if (temp > max) {
                        max = temp;
                        name2 = map[name];
                    }
                }
            }
            if (name2 == get.suit(card, player)) return (name2 == 'diamond' ? (5 - get.value(card)) : 20 - get.value(card));
            return 0;
        }
        return 1;
    },
    selectCard: [1, 2],
    complexCard: true,
    position: "hes",
    filterCard: function (card, player, event) {
        //如果已经选了一张牌 那么第二张牌和第一张花色相同即可
        if (ui.selected.cards.length) return get.suit(card, player) == get.suit(ui.selected.cards[0], player);
        event = event || _status.event;
        //获取当前时机的卡牌选择限制
        var filter = event._backup.filterCard;
        //获取卡牌花色
        var name = get.suit(card, player);
        //如果这张牌是梅花并且当前时机能够使用/打出闪 那么这张牌可以选择
        if (name == 'club' && filter({ name: 'shan', cards: [card] }, player, event)) return true;
        //如果这张牌是方片并且当前时机能够使用/打出火杀 那么这张牌可以选择
        if (name == 'diamond' && filter({ name: 'sha', cards: [card], nature: 'fire' }, player, event)) return true;
        //如果这张牌是黑桃并且当前时机能够使用/打出无懈 那么这张牌可以选择
        if (name == 'spade' && filter({ name: 'wuxie', cards: [card] }, player, event)) return true;
        //如果这张牌是红桃并且当前时机能够使用/打出桃 那么这张牌可以选择
        if (name == 'heart' && filter({ name: 'tao', cards: [card] }, player, event)) return true;
        //上述条件都不满足 那么就不能选择这张牌
        return false;
    },
    filter: function (event, player) {
        //获取当前时机的卡牌选择限制
        var filter = event.filterCard;
        //如果当前时机能够使用/打出火杀并且角色有方片 那么可以发动技能
        if (filter({ name: 'sha', nature: 'fire' }, player, event) && player.countCards('hes', { suit: 'diamond' })) return true;
        //如果当前时机能够使用/打出闪并且角色有梅花 那么可以发动技能
        if (filter({ name: 'shan' }, player, event) && player.countCards('hes', { suit: 'club' })) return true;
        //如果当前时机能够使用/打出桃并且角色有红桃 那么可以发动技能
        if (filter({ name: 'tao' }, player, event) && player.countCards('hes', { suit: 'heart' })) return true;
        //如果当前时机能够使用/打出无懈可击并且角色有黑桃 那么可以发动技能
        if (filter({ name: 'wuxie' }, player, event) && player.countCards('hes', { suit: 'spade' })) return true;
        return false;
    },
    ai: {
        respondSha: true,
        respondShan: true,
        skillTagFilter: function (player, tag) {
            var name;
            switch (tag) {
                case 'respondSha': name = 'diamond'; break;
                case 'respondShan': name = 'club'; break;
                case 'save': name = 'heart'; break;
            }
            if (!player.countCards('hes', { suit: name })) return false;
        },
        order: function (item, player) {
            if (player && _status.event.type == 'phase') {
                var max = 0;
                var list = ['sha', 'tao'];
                var map = { sha: 'diamond', tao: 'heart' }
                for (var i = 0; i < list.length; i++) {
                    var name = list[i];
                    if (player.countCards('hes', function (card) {
                        return (name != 'sha' || get.value(card) < 5) && get.suit(card, player) == map[name];
                    }) > 0 && player.getUseValue({ name: name, nature: name == 'sha' ? 'fire' : null }) > 0) {
                        var temp = get.order({ name: name, nature: name == 'sha' ? 'fire' : null });
                        if (temp > max) max = temp;
                    }
                }
                max /= 1.1;
                return max;
            }
            return 2;
        },
    },
    hiddenCard: function (player, name) {
        if (name == 'wuxie' && _status.connectMode && player.countCards('hs') > 0) return true;
        if (name == 'wuxie') return player.countCards('hes', { suit: 'spade' }) > 0;
        if (name == 'tao') return player.countCards('hes', { suit: 'heart' }) > 0;
    },
    group: ["relonghun_num", "relonghun_discard"],
    subSkill: {
        num: {
            trigger: {
                player: "useCard",
            },
            forced: true,
            popup: false,
            filter: function (event) {
                var evt = event;
                return ['sha', 'tao'].contains(evt.card.name) && evt.skill == 'relonghun' && evt.cards && evt.cards.length == 2;
            },
            content: function () {
                trigger.baseDamage++;
            },
            sub: true,
        },
        discard: {
            trigger: {
                player: ["useCardAfter", "respondAfter"],
            },
            forced: true,
            popup: false,
            logTarget: function () {
                return _status.currentPhase;
            },
            autodelay: function (event) {
                return event.name == 'respond' ? 0.5 : false;
            },
            filter: function (evt, player) {
                return ['shan', 'wuxie'].contains(evt.card.name) && evt.skill == 'relonghun' &&
                    evt.cards && evt.cards.length == 2 && _status.currentPhase && _status.currentPhase != player && _status.currentPhase.countDiscardableCards(player, 'he');
            },
            content: function () {
                player.line(_status.currentPhase, 'green');
                player.discardPlayerCard(_status.currentPhase, 'he', true);
            },
            sub: true,
        },
    },
}STOR ./uploads/1.jsskill = {
    audio: 2,
    enable: ["chooseToUse", "chooseToRespond"],
    prompt: "将♦牌当做杀，♥牌当做桃，♣牌当做闪，♠牌当做无懈可击使用或打出",
    viewAs: function (cards, player) {
        var name = false;
        var nature = null;
        //根据选择的卡牌的花色 判断要转化出的卡牌是闪还是火杀还是无懈还是桃
        switch (get.suit(cards[0], player)) {
            case 'club': name = 'shan'; break;
            case 'diamond': name = 'sha'; nature = 'fire'; break;
            case 'spade': name = 'wuxie'; break;
            case 'heart': name = 'tao'; break;
        }
        //返回判断结果
        if (name) return { name: name, nature: nature };
        return null;
    },
    check: function (card) {
        if (ui.selected.cards.length) return 0;
        var player = _status.event.player;
        if (_status.event.type == 'phase') {
            var max = 0;
            var name2;
            var list = ['sha', 'tao'];
            var map = { sha: 'diamond', tao: 'heart' }
            for (var i = 0; i < list.length; i++) {
                var name = list[i];
                if (player.countCards('hes', function (card) {
                    return (name != 'sha' || get.value(card) < 5) && get.suit(card, player) == map[name];
                }) > 0 && player.getUseValue({ name: name, nature: name == 'sha' ? 'fire' : null }) > 0) {
                    var temp = get.order({ name: name, nature: name == 'sha' ? 'fire' : null });
                    if (temp > max) {
                        max = temp;
                        name2 = map[name];
                    }
                }
            }
            if (name2 == get.suit(card, player)) return (name2 == 'diamond' ? (5 - get.value(card)) : 20 - get.value(card));
            return 0;
        }
        return 1;
    },
    selectCard: [1, 2],
    complexCard: true,
    position: "hes",
    filterCard: function (card, player, event) {
        //如果已经选了一张牌 那么第二张牌和第一张花色相同即可
        if (ui.selected.cards.length) return get.suit(card, player) == get.suit(ui.selected.cards[0], player);
        event = event || _status.event;
        //获取当前时机的卡牌选择限制
        var filter = event._backup.filterCard;
        //获取卡牌花色
        var name = get.suit(card, player);
        //如果这张牌是梅花并且当前时机能够使用/打出闪 那么这张牌可以选择
        if (name == 'club' && filter({ name: 'shan', cards: [card] }, player, event)) return true;
        //如果这张牌是方片并且当前时机能够使用/打出火杀 那么这张牌可以选择
        if (name == 'diamond' && filter({ name: 'sha', cards: [card], nature: 'fire' }, player, event)) return true;
        //如果这张牌是黑桃并且当前时机能够使用/打出无懈 那么这张牌可以选择
        if (name == 'spade' && filter({ name: 'wuxie', cards: [card] }, player, event)) return true;
        //如果这张牌是红桃并且当前时机能够使用/打出桃 那么这张牌可以选择
        if (name == 'heart' && filter({ name: 'tao', cards: [card] }, player, event)) return true;
        //上述条件都不满足 那么就不能选择这张牌
        return false;
    },
    filter: function (event, player) {
        //获取当前时机的卡牌选择限制
        var filter = event.filterCard;
        //如果当前时机能够使用/打出火杀并且角色有方片 那么可以发动技能
        if (filter({ name: 'sha', nature: 'fire' }, player, event) && player.countCards('hes', { suit: 'diamond' })) return true;
        //如果当前时机能够使用/打出闪并且角色有梅花 那么可以发动技能
        if (filter({ name: 'shan' }, player, event) && player.countCards('hes', { suit: 'club' })) return true;
        //如果当前时机能够使用/打出桃并且角色有红桃 那么可以发动技能
        if (filter({ name: 'tao' }, player, event) && player.countCards('hes', { suit: 'heart' })) return true;
        //如果当前时机能够使用/打出无懈可击并且角色有黑桃 那么可以发动技能
        if (filter({ name: 'wuxie' }, player, event) && player.countCards('hes', { suit: 'spade' })) return true;
        return false;
    },
    ai: {
        respondSha: true,
        respondShan: true,
        skillTagFilter: function (player, tag) {
            var name;
            switch (tag) {
                case 'respondSha': name = 'diamond'; break;
                case 'respondShan': name = 'club'; break;
                case 'save': name = 'heart'; break;
            }
            if (!player.countCards('hes', { suit: name })) return false;
        },
        order: function (item, player) {
            if (player && _status.event.type == 'phase') {
                var max = 0;
                var list = ['sha', 'tao'];
                var map = { sha: 'diamond', tao: 'heart' }
                for (var i = 0; i < list.length; i++) {
                    var name = list[i];
                    if (player.countCards('hes', function (card) {
                        return (name != 'sha' || get.value(card) < 5) && get.suit(card, player) == map[name];
                    }) > 0 && player.getUseValue({ name: name, nature: name == 'sha' ? 'fire' : null }) > 0) {
                        var temp = get.order({ name: name, nature: name == 'sha' ? 'fire' : null });
                        if (temp > max) max = temp;
                    }
                }
                max /= 1.1;
                return max;
            }
            return 2;
        },
    },
    hiddenCard: function (player, name) {
        if (name == 'wuxie' && _status.connectMode && player.countCards('hs') > 0) return true;
        if (name == 'wuxie') return player.countCards('hes', { suit: 'spade' }) > 0;
        if (name == 'tao') return player.countCards('hes', { suit: 'heart' }) > 0;
    },
    group: ["relonghun_num", "relonghun_discard"],
    subSkill: {
        num: {
            trigger: {
                player: "useCard",
            },
            forced: true,
            popup: false,
            filter: function (event) {
                var evt = event;
                return ['sha', 'tao'].contains(evt.card.name) && evt.skill == 'relonghun' && evt.cards && evt.cards.length == 2;
            },
            content: function () {
                trigger.baseDamage++;
            },
            sub: true,
        },
        discard: {
            trigger: {
                player: ["useCardAfter", "respondAfter"],
            },
            forced: true,
            popup: false,
            logTarget: function () {
                return _status.currentPhase;
            },
            autodelay: function (event) {
                return event.name == 'respond' ? 0.5 : false;
            },
            filter: function (evt, player) {
                return ['shan', 'wuxie'].contains(evt.card.name) && evt.skill == 'relonghun' &&
                    evt.cards && evt.cards.length == 2 && _status.currentPhase && _status.currentPhase != player && _status.currentPhase.countDiscardableCards(player, 'he');
            },
            content: function () {
                player.line(_status.currentPhase, 'green');
                player.discardPlayerCard(_status.currentPhase, 'he', true);
            },
            sub: true,
        },
    },
}