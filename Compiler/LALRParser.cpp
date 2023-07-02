#include "LALRParser.h"


LALRParser::LALRParser(GrammarPtr gp) : grammarP(gp) {
	computeParsingTable();
	//printParsingTable();
}

void LALRParser::computeParsingTable() {
	stack<int> waitList; // 待处理栈


	// 创建初始项集 在创建每个项集时 创建项集的唯一标识，用于确定是否为已存在项集
	ItemSetPtr iSetStart = make_shared<ItemSet>(ItemSet(make_shared<Item>(Item(grammarP->rules[0], 0, 0, grammarP)), grammarP, 0));
	// 添加其后继符号
	iSetStart->items[0]->addSuc(gSymbolTable.endMark);
	
	iSetStart->expand();
	idMap[iSetStart->id].insert(0);
	itemSets.push_back(iSetStart);

	waitList.push(0);

	// 计算项集的 移进 情况
	while (!waitList.empty()) {
		// 取出一个待处理项集
		int nowItemSetIndex = waitList.top(); waitList.pop();
		ItemSetPtr nowSet = itemSets[nowItemSetIndex];
		map<symbol, set<int>> shiftMap;

		// 检查规约 以及移进
		for (int i = 0; i < nowSet->items.size(); i++) {
			ItemPtr ip = nowSet->items[i];
			// 如果可以被规约
			if (ip->next() == -3) {
				if (pt.insertAction(nowItemSetIndex, ip->successors, Action(reduce, ip->rp->origin, grammarP->ruleIndex(ip->rp->origin), ip->index))) {
					// 规约成功
				}
				else {
					// 规约失败 存在冲突
					//cout << "--- reduce error ---\n";
				}
			}
			// 如果可以移进
			else {
				//对该项集下的移进项按next分组
				shiftMap[ip->next()].insert(i);
			}
		}

		// 对移进项每一组创建一个新的临时项集
		for (auto p : shiftMap) {
			// 创建项集 创建项集的唯一标识，用于确定是否为已存在项集
			int thisNewSetIndex = itemSets.size();
			ItemSetPtr newSet = make_shared<ItemSet>(ItemSet(grammarP, thisNewSetIndex));

			for (auto index : p.second) {
				ItemPtr ip = nowSet->items[index];

				newSet->addItem(ip->rp, ip->index, ip->dot + 1, ip->successors);
				newSet->id.insert({ grammarP->ruleIndex(ip->rp->origin), ip->index, ip->dot + 1 });

			}

			newSet->expand();

			// 这是新的项集
			if (idMap.find(newSet->id) == idMap.end()) {
				itemSets.push_back(newSet);
				idMap[newSet->id].insert(thisNewSetIndex);
				waitList.push(thisNewSetIndex);
				
				//cout << thisNewSetIndex << '\n';

				if (pt.insertAction(nowItemSetIndex, p.first, Action(shift, thisNewSetIndex, 0, 0))) {
					// 移进成功
				}
				else {
					// 移进失败 存在冲突
					//cout << "------------------------ new shift error ---\n";
				}
			}
			// 项集已存在，尝试合并
			else {
				// 相同的itemSet索引
				int sameIndex = -1;
				// 遍历该id对应的项集
				for (auto existItemSetIndex : idMap[newSet->id]) {

					// 如果newSet与其对应的itemSet后继集合相同 则说明是同一状态
					ItemSetPtr existIsp = itemSets[existItemSetIndex];
					bool isSame = true;
					for (ItemPtr ip1 : newSet->items) {
						for (ItemPtr ip2 : existIsp->items) {
							if (*ip1 == *ip2) {
								if (ip1->successors != ip2->successors) isSame = false;
								break;
							}
						}
						if (!isSame) break;
					}
					if (isSame) {
						sameIndex = existItemSetIndex;
						break;
					}
				}

				// 存在同一状态的项集的话 直接移进就好
				if (sameIndex != -1) {
					if (pt.insertAction(nowItemSetIndex, p.first, Action(shift, sameIndex, 0, 0))) {
						// 移进成功
					}
					else {
						// 移进失败 存在冲突
						//cout << "------------------------ exist shift error ---\n";
					}
				}
				// 不存在同一状态的项集的话 创建新的状态
				else {
					itemSets.push_back(newSet);
					idMap[newSet->id].insert(thisNewSetIndex);
					waitList.push(thisNewSetIndex);

					//cout << thisNewSetIndex << '\n';

					if (pt.insertAction(nowItemSetIndex, p.first, Action(shift, thisNewSetIndex, 0, 0))) {
						// 移进成功
					}
					else {
						// 移进失败 存在冲突
						//cout << "------------------------ new shift error ---\n";
					}
				}
			}

		}
	}
}


void ItemSet::expand() {
	stack<symbol> wSet; // 待处理产生式
	unordered_map<tuple<int, int, int>, int, TupleHash> fmap; // 标记符号是否有被作为origin处理过

	// 加入初始的产生式
	for (int i = 0; i < items.size(); i++) {
		wSet.push(i);
		
		fmap[{ grammarP->ruleIndex(items[i]->rp->origin), items[i]->index, items[i]->dot}] = i;
		id.insert({ grammarP->ruleIndex(items[i]->rp->origin), items[i]->index, items[i]->dot });
	}

	while (!wSet.empty()) {
		// 每次取出一个待处理产生式
		int i = wSet.top(); wSet.pop();

		symbol s = items[i]->next();
		if (s != -3) {
			// dot后为非终结符则展开加入项集中，注意后继符号
			if (!gSymbolTable.isTerminal(s)) {

				int ruleIndex = grammarP->ruleIndex(s);

				// 同一个origin 可以展开的产生式
				rulePtr r = grammarP->rules[ruleIndex];

				// 遍历每一种展开式
				for (int j = 0; j < r->expansions.size(); j++) {
					// 如果未存在，则向 ItemSet 中 加入新的 Item
					if (fmap.find(make_tuple(ruleIndex, j, 0)) == fmap.end()) {
						ItemPtr ip = make_shared<Item>(Item(r, j, 0, grammarP));
						set<symbol> suc;
						items[i]->succ(suc);
						ip->addSuc(suc);
						id.insert({ ruleIndex, j, 0 });
						// 将新加入的 Item 加入待处理栈
						wSet.push(items.size());

						// 加索引标记已处理过的产生式
						fmap[{ruleIndex, j, 0}] = items.size();

						items.push_back(ip);
						
					}
					// 如果已经存在 则尝试完善 该 Item 后继符号 同时与其相关的 Item 均需要修改
					else {
						set<symbol> suc;
						items[i]->succ(suc);
						if (items[fmap[{ruleIndex, j, 0}]]->addSuc(suc)) {
							wSet.push(fmap[{ruleIndex, j, 0}]);
						};
					}
				}
			}
		}
	}
}